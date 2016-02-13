//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CTimeLogger.hpp"
#include "CDocumentBinarizer.hpp"

void CImageProcessor::StartProcessingChain(const CImage& img)
{
	CImage extentImage = img.GetExtentImage(BinaryWindowSize);
	
	_mainImage = CImagePatch();
	_mainImage.SetGrayImage(extentImage);
	BuildAndShowBinImage(extentImage, false);
	BuildAndShowSdImage(extentImage, false);
    
#if TEST_BLUR_METRICS || PROCESS_IMAGE
	WindowDidSelectPatch(_window.GetName(), {0, 0, 0, 0});
	exit(0);
#else
	ConfigureWindow(extentImage);
#endif
}

void CImageProcessor::WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect)
{
#if SHOW_BLUR_MAP
	ProcessShowBlurMap(patchRect);
#elif HIGHLIGHT_SIMILAR_PATCHES
	ProcessHighlightSimilarPatches(patchRect);
#elif SHOW_SORTED_SIMILAR
	ProcessShowSortedSimilar(patchRect);
#elif REPLACE_SIMILAR_PATCHES
	ProcessReplaceSimilarPatches(patchRect);
#elif TEST_BLUR_METRICS
	ProcessTestBlurMetrics();
#endif
}

#pragma mark - Private

void CImageProcessor::ProcessFixImage()
{
    // извлекаем все патчи изображения
    std::deque<CImagePatch> patches = FetchPatches({0, 0, MaxPatchSideSize, MaxPatchSideSize});
    // кластеризуем их
    std::map<uint64, std::deque<CImagePatch>> clusters = FetchClusters(patches);

    
}

#pragma mark - Utils

void CImageProcessor::BuildAndShowBinImage(const CImage &img, bool show)
{
	// строим бинаризованное изображение
	CTimeLogger::StartLogging();
	
	CDocumentBinarizer binarizer(BinaryWindowSize, BinMethod, 2.f);
	CImage blurredImage;
	cv::bilateralFilter(img, blurredImage, 2, 1, 1);
	CImage binarizedImage;
	binarizedImage = binarizer.Binarize(img.GetExtentImage(BinaryWindowSize));
	if (show) {
		_binarizedWindow.ShowAndUpdate(binarizedImage);
	}
	
	CTimeLogger::Print("Binarization: ");
	
	binarizedImage.Save("bin");
	
	_mainImage.SetBinImage(binarizedImage);
}

void CImageProcessor::BuildAndShowSdImage(const CImage &img, bool show)
{
	// строим sd изображение
	CTimeLogger::StartLogging();
	
	CImage sdImage = img.GetSDImage({MaxPatchSideSize, MaxPatchSideSize});
	
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
	selectedPatch.PHash();
	selectedPatch.AvgHash();
	
	std::cout << "-------\nSelectedPatch:\n" << selectedPatch << "\n-------" <<std::endl;
	
	return selectedPatch;
}

std::deque<CImagePatch> CImageProcessor::FetchPatches(const cv::Rect& patchRect)
{
	CTimeLogger::StartLogging();
	
	std::deque<CImagePatch> patches;
	cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
	_mainImage.SetGrayImage(_mainImage.GrayImage().GetExtentImage(patchSize));
	_mainImage.SetBinImage(_mainImage.BinImage().GetExtentImage(patchSize));
	_mainImage.SetSdImage(_mainImage.SdImage().GetExtentImage(patchSize));
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

std::deque<CImagePatch> CImageProcessor::FindSimilarPatches(CImagePatch& targetPatch, std::deque<CImagePatch>& patches)
{
	CTimeLogger::StartLogging();
	
	auto comparePatches = [](CImagePatch& patch1, CImagePatch& patch2) {
		if (ClusteringMethod == TPatchClusteringMethodPHash) {
			return utils::hamming<uint64>(patch1.PHash(), patch2.PHash());
		} else if (ClusteringMethod == TPatchClusteringMethodAvgHash) {
			return utils::hamming<uint64>(patch1.AvgHash(), patch2.AvgHash());
		}
	};
	
	//	static auto search = [&](CImagePatch* begin, CImagePatch* end) {
	//		std::deque<CImagePatch> similarPatches;
	//		CImageComparator imgComparator(CompMetric);
	//		int eps = CompEpsForCompMetric(CompMetric);
	//		for (auto it = begin; it != end; ++it) {
	//			CImagePatch patch(*it);
	//			if (comparePatches(patch, targetPatch) == 0) {
	//				int distance = imgComparator.Compare(targetPatch, patch);
	//				patch.distanceToTarget = distance;
	//				if (distance < eps) {
	//					patch.BlurValue(BlurMeasureMethod);
	//					similarPatches.push_back(patch);
	//				}
	//			}
	//		}
	//		return similarPatches;
	//	};
	//
	//	auto firstHalfSearch = std::async(search, &patches[0], &patches[patches.size()/2]);
	//	auto secondHalfSearch = std::async(search, &patches[patches.size()/2], &patches[patches.size()]);
	//
	//	auto similarPatches = firstHalfSearch.get();
	//	similarPatches.insert(firstHalfSearch.get().end(), secondHalfSearch.get().begin(), secondHalfSearch.get().end());
	
	std::deque<CImagePatch> similarPatches;
	CImageComparator imgComparator(CompMetric);
	int eps = CompEpsForCompMetric(CompMetric);
	for (CImagePatch& patch: patches) {
		if (comparePatches(patch, targetPatch) == 0) {
			int distance = imgComparator.Compare(targetPatch, patch);
			patch.distanceToTarget = distance;
			if (distance < eps) {
				patch.BlurValue(BlurMeasureMethod);
				similarPatches.push_back(patch);
			}
		}
	}
	
	std::cout << "Similar patches found: " << similarPatches.size() << std::endl;
	CTimeLogger::Print("Patch search: ");
	
	return similarPatches;
}

std::map<uint64, std::deque<CImagePatch> > CImageProcessor::FetchClusters(std::deque<CImagePatch>& patches)
{
	CTimeLogger::StartLogging();
	
	std::map<uint64, std::deque<CImagePatch>> clusters;
	
	auto clusteringHash = [](CImagePatch& patch) {
		if (ClusteringMethod == TPatchClusteringMethodPHash) {
			return patch.PHash();
		} else if (ClusteringMethod == TPatchClusteringMethodAvgHash) {
			return patch.AvgHash();
		}
	};
	
	for (CImagePatch& patch: patches) {
		auto cluster = clusters.find(clusteringHash(patch));
		if (cluster == clusters.end()) {
			clusters[patch.AvgHash()] = std::deque<CImagePatch>(1, patch);
		} else {
			(*cluster).second.push_back(patch);
		}
	}
	
	CTimeLogger::Print("Patch clustering: ");
	
	return clusters;
}

void CImageProcessor::AddBlurValueRect(std::deque<DrawableRect>& rects, CImagePatch& imagePatch)
{
	double colorComp = imagePatch.BlurValue(BlurMeasureMethod);
	cv::Scalar color = RGB(colorComp, colorComp, colorComp);
	rects.push_back({imagePatch.GetFrame(), color, CV_FILLED});
}

int CImageProcessor::CompEpsForCompMetric(TImageCompareMetric metric)
{
	switch (metric) {
		case TImageCompareMetricL1:
			return ComparisonEpsL1;
		case TImageCompareMetricL2:
			return ComparisonEpsL2;
		default:
			break;
	}
	
	return 0;
}
