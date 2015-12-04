//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CTimeLogger.hpp"
#include "CDocumentBinarizer.hpp"

#ifdef DEBUG
#define SHOW_BLUR_MAP 0
#define HIGHLIGHT_SIMILAR_PATCHES 0
#define SHOW_SORTED_SIMILAR 0
#define REPLACE_SIMILAR_PATCHES 0
#define FIX_IMAGE_STUPID 1
#endif

void CImageProcessor::StartProcessingChain(const CImage& img)
{
	CImage extentImage = utils::ExtentImage(img, BinaryWindowSize);
	
	_mainImage = CImagePatch();
	_mainImage.SetGrayImage(extentImage);
	BuildAndShowBinImage(extentImage, false);
	BuildAndShowSdImage(extentImage, false);
	
	ConfigureWindow(extentImage);
}

void CImageProcessor::WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect)
{
#if SHOW_BLUR_MAP
	ProcessShowBlurMap(patchRect);
#elif HIGHLIGHT_SIMILAR_PATCHES
	ProcessShowSimilarPatches(patchRect);
#elif SHOW_SORTED_SIMILAR
	ProcessShowSortedSimilar(patchRect);
#elif REPLACE_SIMILAR_PATCHES
	ProcessReplaceSimilarPatches(patchRect);
#elif FIX_IMAGE_STUPID
	ProcessFixImageStupid();
#endif
}

#pragma mark - Private

void CImageProcessor::ProcessShowBlurMap(const cv::Rect &patchRect)
{
	CImagePatch selectedPatch = FetchPatch(patchRect);
	std::vector<CImagePatch> patches = FetchPatches(patchRect);
	CTimeLogger::StartLogging("Show blur map:\n");
	
	std::vector<DrawableRect> rectsToDraw;
	for (int i = 0; i < patches.size(); i++) {
		AddBlurValueRect(rectsToDraw, patches[i]);
	}
	
	_window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessHighlightSimilarPatches(const cv::Rect &patchRect)
{
	CImagePatch selectedPatch = FetchPatch(patchRect);
	
	std::vector<CImagePatch> patches = FetchPatches(patchRect);
	
	CTimeLogger::StartLogging("Highlight patches:\n");
	
	std::vector<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
	std::vector<DrawableRect> rectsToDraw;
	int good = 0;
	for (int i = 0; i < similarPatches.size(); i++) {
		// чем больше размытия, тем темнее рамка вокруг патча
		cv::Scalar color = RGB(0, similarPatches[i].BlurValue(BlurMeasureMethod), 0);
		rectsToDraw.push_back({similarPatches[i].GetFrame(), color});
		good++;
		
		std::cout << "\t" << std::setw(4) << good << ". Frame: " << patches[i].GetFrame() << std::endl;
	}
	
	std::cout << "\nGood patches: " << good << std::endl;
	CTimeLogger::Print("Patches to highlight search:");
	
	_window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessShowSortedSimilar(const cv::Rect &patchRect)
{
	CImagePatch selectedPatch = FetchPatch(patchRect);
	std::vector<CImagePatch> patches = FetchPatches(patchRect);
	
	std::vector<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
	
	std::sort(similarPatches.begin(), similarPatches.end(), LessSimilarity());
	
	CImage similarityDecreaseImg;
	for (int i = 0; i < similarPatches.size(); i++) {
		CImage temp;
		cv::hconcat(similarPatches[i].BinImage(), similarPatches[i].GrayImage(), temp);
		if (similarityDecreaseImg.cols > 0 && similarityDecreaseImg.rows > 0) {
			cv::vconcat(similarityDecreaseImg, temp, similarityDecreaseImg);
		} else {
			similarityDecreaseImg = temp;
		}
	}
	
	std::sort(similarPatches.begin(), similarPatches.end(), LessBlur());
	CImage blurIncreaseImg;
	for (int i = 0; i < similarPatches.size(); i++) {
		CImage temp;
		cv::hconcat(similarPatches[i].BinImage(), similarPatches[i].GrayImage(), temp);
		if (blurIncreaseImg.cols > 0 && blurIncreaseImg.rows > 0) {
			cv::vconcat(blurIncreaseImg, temp, blurIncreaseImg);
		} else {
			blurIncreaseImg = temp;
		}
	}
	
	utils::SaveImage(SaveImgPath + "similarityDecrease.jpg", similarityDecreaseImg);
	utils::SaveImage(SaveImgPath + "blurIncrease.jpg", blurIncreaseImg);
}

void CImageProcessor::ProcessReplaceSimilarPatches(const cv::Rect &patchRect)
{
	CImagePatch selectedPatch = FetchPatch(patchRect);
	std::vector<CImagePatch> patches = FetchPatches(patchRect);
	
	// извлекаем похожие патчи
	std::vector<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
	
	CTimeLogger::StartLogging();
	
	// сортируем по резкости
	std::sort(similarPatches.begin(), similarPatches.end(), LessBlur());
	
	CImagePatch sharpPatch = similarPatches[0];
	std::vector<DrawableRect> rectsToDraw;
	CImage grayImage = _mainImage.GrayImage();
	
	// замещаем участки изображения
	for (int i = 1; i < similarPatches.size(); i++) {
		CImage temp = grayImage(similarPatches[i].GetFrame());
		sharpPatch.GrayImage().copyTo(temp);
		cv::Scalar color = RGB(0, similarPatches[i].BlurValue(BlurMeasureMethod), 0);
		rectsToDraw.push_back({similarPatches[i].GetFrame(), color});
	}
	_mainImage.SetGrayImage(grayImage);
	
	CTimeLogger::Print("Image fix:");
	
	_window.Update(_mainImage.GrayImage());
	//	_window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessFixImageStupid()
{
	std::vector<CImagePatch> patches = FetchPatches({0, 0, MaxPatchSideSize, MaxPatchSideSize});
	std::map<int64, std::deque<CImagePatch>> clusters = FetchClusters(patches);
	
	int counter = 0;
	for (auto it = clusters.begin() ; it != clusters.end(); ++it) {
		if (counter == clusters.size() - 1) {
			break;
		}
		std::deque<CImagePatch> patchCluster = (*it).second;
		std::sort(patchCluster.begin(), patchCluster.end(), LessBlur());
		counter++;
	}
	
	CImage grayImage = _mainImage.GrayImage();
	counter = 0;
	for (auto cluster: clusters) {
		CImagePatch sharpPatch = cluster.second[0];
		for (CImagePatch patch: cluster.second) {
			CImage temp = grayImage(patch.GetFrame());
			sharpPatch.GrayImage().copyTo(temp);
		}
	}
	
	_window.Update(_mainImage.GrayImage());
	BuildAndShowBinImage(_mainImage.GrayImage(), true);
}

void CImageProcessor::BuildAndShowBinImage(const CImage &img, bool show)
{
	// строим бинаризованное изображение
	CTimeLogger::StartLogging();
	
	CDocumentBinarizer binarizer(BinaryWindowSize, BinMethod, 2.f);
	CImage blurredImage;
	cv::bilateralFilter(img, blurredImage, 2, 1, 1);
	CImage binarizedImage;
	binarizedImage = binarizer.Binarize(utils::ExtentImage(img, BinaryWindowSize));
	
	if (show) {
		_binarizedWindow.ShowAndUpdate(binarizedImage);
	}
	
	CTimeLogger::Print("Binarization: ");
	
	_mainImage.SetBinImage(binarizedImage);
}

void CImageProcessor::BuildAndShowSdImage(const CImage &img, bool show)
{
	// строим sd изображение
	CTimeLogger::StartLogging();
	
	CImage sdImage;
	sdImage = utils::SDFilter(img, cv::Size(MaxPatchSideSize, MaxPatchSideSize));
	
	if (show) {
		_debugWindow.ShowAndUpdate(sdImage);
	}
	
	CTimeLogger::Print("SD filter: ");
	
	_mainImage.SetSdImage(sdImage);
}

void CImageProcessor::ConfigureWindow(const CImage& img)
{
	// изображение для вывода
	img.copyTo(_displayImage);
	// делаем цветным
	cv::cvtColor(_displayImage, _displayImage, CV_GRAY2RGBA);
	
	_window.SetMaxBoxSideSize(MaxPatchSideSize);
	_window.SetOriginalImage(_displayImage);
	_window.ShowAndUpdate(_displayImage);
	_window.StartObservingMouse();
	_window.ObserveKeyboard();
}

CImagePatch CImageProcessor::FetchPatch(const cv::Rect &patchRect)
{
	cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
	CDocumentBinarizer binarizer = CDocumentBinarizer(patchSize);
	
	CImagePatch selectedPatch = CImagePatch();
	selectedPatch.SetGrayImage(CImage(_mainImage.GrayImage(), patchRect));
	selectedPatch.SetBinImage(CImage(_mainImage.BinImage(), patchRect));
	selectedPatch.BlurValue(BlurMeasureMethod);
	selectedPatch.StandartDeviation();
	if (CompMetric == TImageCompareMetricPHash) {
		selectedPatch.PHash();
	} else if (CompMetric == TImageCompareMetricAvgHash) {
		selectedPatch.AvgHash();
	}
	
	std::cout << "-------\nSelectedPatch:\n" << selectedPatch << "\n-------" <<std::endl;
	
	return selectedPatch;
}

std::vector<CImagePatch> CImageProcessor::FetchPatches(const cv::Rect& patchRect)
{
	CTimeLogger::StartLogging();
	
	std::vector<CImagePatch> patches;
	cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
	_mainImage.SetGrayImage(utils::ExtentImage(_mainImage.GrayImage(), patchSize));
	_mainImage.SetBinImage(utils::ExtentImage(_mainImage.BinImage(), patchSize));
	_mainImage.SetSdImage(utils::ExtentImage(_mainImage.SdImage(), patchSize));
	CImage grayImage = _mainImage.GrayImage();
	CImage binImage = _mainImage.BinImage();
	CImage sdImage = _mainImage.SdImage();
	
	CImage::CPatchIterator patchIterator = grayImage.GetPatchIterator(patchSize, PatchOffset);
	CImage::CPatchIterator binPatchIterator = binImage.GetPatchIterator(patchSize, PatchOffset);
	CImage::CPatchIterator sdPatchIterator = sdImage.GetPatchIterator(patchSize, PatchOffset);
	
	while (patchIterator.HasNext()) {
		CImagePatch imgPatch;
		imgPatch.SetBinImage(binPatchIterator.GetNext());
		imgPatch.SetGrayImage(patchIterator.GetNext());
		imgPatch.SetSdImage(sdPatchIterator.GetNext());
		patches.push_back(imgPatch);
	}
	
	CTimeLogger::Print("Patch fetching: ");
	
	return patches;
}

std::vector<CImagePatch> CImageProcessor::FindSimilarPatches(CImagePatch& patch, std::vector<CImagePatch>& patches)
{
	CTimeLogger::StartLogging();
	
	std::vector<CImagePatch> similarPatches;
	CImageComparator imgComparator(CompMetric);
	int eps = CompEpsForCompMetric(CompMetric);
	for (int i = 0; i < patches.size(); i++) {
		if (utils::hamming(patches[i].AvgHash(), patch.AvgHash()) == 0) {
			int distance = imgComparator.Compare(patch, patches[i]);
			patches[i].distanceToTarget = distance;
			if (distance < eps) {
				patches[i].BlurValue(BlurMeasureMethod);
				similarPatches.push_back(patches[i]);
			}
		}
	}
	
	std::cout << "Similar patches found: " << similarPatches.size() << std::endl;
	CTimeLogger::Print("Patch search: ");
	
	return similarPatches;
}

std::map<int64, std::deque<CImagePatch> > CImageProcessor::FetchClusters(std::vector<CImagePatch>& patches)
{
	CTimeLogger::StartLogging();
	
	std::map<int64, std::deque<CImagePatch> > clusters = std::map<int64, std::deque<CImagePatch> >();
	
	for (CImagePatch patch: patches) {
		auto cluster = clusters.find(patch.AvgHash());
		if (cluster == clusters.end()) {
			clusters[patch.AvgHash()] = std::deque<CImagePatch>(1, patch);
		} else {
			(*cluster).second.push_back(patch);
		}
	}
	
	CTimeLogger::Print("Patch clustering: ");
	
	return clusters;
}

void CImageProcessor::AddBlurValueRect(std::vector<DrawableRect>& rects, CImagePatch& imagePatch)
{
	double colorComp = imagePatch.BlurValue(BlurMeasureMethod);
	cv::Scalar color = RGB(colorComp, colorComp, colorComp);
	rects.push_back({imagePatch.GetFrame(), color, CV_FILLED});
}


#pragma mark - Utils

int CImageProcessor::CompEpsForCompMetric(TImageCompareMetric metric)
{
	switch (metric) {
		case TImageCompareMetricL1:
			return ComparisonEpsL1;
		case TImageCompareMetricL2:
			return ComparisonEpsL2;
		case TImageCompareMetricPHash:
			return ComparisonEpsPHash;
		case TImageCompareMetricAvgHash:
			return ComparisonEpsAvgHash;
		default:
			break;
	}
	
	return 0;
}
