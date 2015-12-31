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
	
#if DRAW_HISTOGRAM || TEST_BLUR_METRICS || FIX_IMAGE_STUPID
	WindowDidSelectPatch(_window.GetName(), {0,0,0,0});
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
	ProcessShowSimilarPatches(patchRect);
#elif SHOW_SORTED_SIMILAR
	ProcessShowSortedSimilar(patchRect);
#elif REPLACE_SIMILAR_PATCHES
	ProcessReplaceSimilarPatches(patchRect);
#elif FIX_IMAGE_STUPID
	ProcessFixImageStupid();
#elif DRAW_HISTOGRAM
	ProcessDrawHistogram();
#elif TEST_BLUR_METRICS
	ProcessTestBlurMetrics();
#endif
}

#pragma mark - Private

void CImageProcessor::ProcessTestBlurMetrics()
{
	int patchSideSize = 25;
	cv::Point origin(20, 25);
	
	auto sortedPatches = std::deque<CImagePatch>();
	auto patches = std::deque<CImagePatch>();
	for (int i = 0; i < 12; i++) {
		CImagePatch patch;
		cv::Rect rect(origin.x, origin.y, patchSideSize, patchSideSize);
		patch.SetBinImage(_mainImage.BinImage()(rect));
		patch.SetSdImage(_mainImage.SdImage()(rect));
		patch.SetGrayImage(_mainImage.GrayImage()(rect));
		std::cout << "Blur value " << patch.BlurValue(TBlurMeasureMethodFFT) << std::endl;
		patches.push_back(patch);
		sortedPatches.push_back(patch);
		origin.y += patchSideSize;
	}
	
	std::sort(sortedPatches.begin(), sortedPatches.end(), LessBlur());
	
	std::cout << std::endl;
	
	CImage result;
	for (int i = 0; i < patches.size(); i++) {
		CImage temp;
		cv::Mat verticalSeparator(patches[i].GetFrame().height, 1, CV_8UC1, cv::Scalar(255));
		cv::hconcat(patches[i].GrayImage(), verticalSeparator, temp);
		cv::hconcat(temp, sortedPatches[i].GrayImage(), temp);
		
		std::cout << "Blur value " << sortedPatches[i].BlurValue(TBlurMeasureMethodFFT) << std::endl;
		if (result.cols > 0 && result.rows > 0) {
			cv::Mat horisontalSeparator(1, patches[i].GetFrame().width * 2 + 1, CV_8UC1, cv::Scalar(255));
			cv::vconcat(result, horisontalSeparator, result);
			cv::vconcat(result, temp, result);
		} else {
			result = temp;
		}
	}

	result.Save("blurTest");
}

void CImageProcessor::ProcessDrawHistogram(const cv::Rect &patchRect)
{
	std::deque<CImagePatch> patches = FetchPatches(patchRect);
	auto clusters = FetchClusters(patches);
	
	int clusterCount = clusters.size();
	
	std::vector<int> hist;
	for (auto& cluster: clusters) {
		
	}
}

void CImageProcessor::ProcessShowBlurMap(const cv::Rect &patchRect)
{
	CImagePatch selectedPatch = FetchPatch(patchRect);
	std::deque<CImagePatch> patches = FetchPatches(patchRect);
	CTimeLogger::StartLogging("Show blur map:\n");
	
	std::deque<DrawableRect> rectsToDraw;
	for (auto& patch: patches) {
		AddBlurValueRect(rectsToDraw, patch);
	}
	
	_window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessHighlightSimilarPatches(const cv::Rect &patchRect)
{
	CImagePatch selectedPatch = FetchPatch(patchRect);
	
	std::deque<CImagePatch> patches = FetchPatches(patchRect);
	
	CTimeLogger::StartLogging("Highlight patches:\n");
	
	std::deque<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
	std::deque<DrawableRect> rectsToDraw;
	int good = 0;
	for (auto& similarPatch: similarPatches) {
		// чем больше размытия, тем темнее рамка вокруг патча
		cv::Scalar color = RGB(0, similarPatch.BlurValue(BlurMeasureMethod), 0);
		rectsToDraw.push_back({similarPatch.GetFrame(), color});
		good++;
		
		std::cout << "\t" << std::setw(4) << good << ". Frame: " << similarPatch.GetFrame() << std::endl;
	}
	
	std::cout << "\nGood patches: " << good << std::endl;
	CTimeLogger::Print("Patches to highlight search:");
	
	_window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessShowSortedSimilar(const cv::Rect &patchRect)
{
	cv::Rect normPatch = {450, 432, 8, 8};
	CImagePatch selectedPatch = FetchPatch(normPatch);
	std::deque<CImagePatch> patches = FetchPatches(normPatch);
	
	auto clusters = FetchClusters(patches);
	
	std::cout << patches.size() << std::endl;
	std::cout << clusters[0].size() << std::endl;
	
	//	CImagePatch selectedPatch = FetchPatch(patchRect);
	//	std::deque<CImagePatch> patches = FetchPatches(patchRect);
	
	std::deque<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
	if (similarPatches.empty()) {
		return;
	}
	
	std::sort(similarPatches.begin(), similarPatches.end(), LessSimilarity());
	
	auto buildImage = [](const std::deque<CImagePatch>& similarPatches) {
		CImage result;
		for (auto& similarPatch: similarPatches) {
			CImage temp;
			cv::hconcat(similarPatch.BinImage(), similarPatch.GrayImage(), temp);
			if (result.cols > 0 && result.rows > 0) {
				cv::vconcat(result, temp, result);
			} else {
				result = temp;
			}
		}
		return result;
	};
	
	CImage similarityDecreaseImg = buildImage(similarPatches);
	std::sort(similarPatches.begin(), similarPatches.end(), LessBlur());
	CImage blurIncreaseImg =  buildImage(similarPatches);
	
	similarityDecreaseImg.Save("similarityDecrease");
	blurIncreaseImg.Save("similarityDecrease");
}

void CImageProcessor::ProcessReplaceSimilarPatches(const cv::Rect &patchRect)
{
	CImagePatch selectedPatch = FetchPatch(patchRect);
	std::deque<CImagePatch> patches = FetchPatches(patchRect);
	
	// извлекаем похожие патчи
	std::deque<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
	
	if (similarPatches.empty()) {
		return;
	}
	
	CTimeLogger::StartLogging();
	
	// сортируем по резкости
	std::sort(similarPatches.begin(), similarPatches.end(), LessBlur());
	
	std::vector<int> labels;
	std::vector<float> data;
	int i = 0;
	for (const CImagePatch& patch: similarPatches) {
		labels.push_back(i);
		data.push_back(patch.distanceToTarget);
	}
	cv::Mat labelsMat(labels, true);
	cv::Mat dataMat(data, true);
	
	cv::TermCriteria::Type termCriteriaType = cv::TermCriteria::Type::EPS;
	cv::TermCriteria criteria(termCriteriaType,  10, 1);
	int k = 5;
	std::vector<float> centers;
	cv::kmeans(dataMat, k, labels, criteria, 10, cv::KMEANS_RANDOM_CENTERS, centers);
	
	if (labelsMat.isContinuous()) {
		labels.assign(labelsMat.datastart, labelsMat.dataend);
	}
	
	CImagePatch sharpPatch = similarPatches[0];
	std::deque<DrawableRect> rectsToDraw;
	CImage grayImage = _mainImage.GrayImage();
	
	// замещаем участки изображения
	for (CImagePatch& similarPatche: similarPatches) {
		CImage temp = grayImage(similarPatche.GetFrame());
		sharpPatch.GrayImage().copyTo(temp);
		cv::Scalar color = RGB(0, similarPatche.BlurValue(BlurMeasureMethod), 0);
		rectsToDraw.push_back({similarPatche.GetFrame(), color});
	}
	_mainImage.SetGrayImage(grayImage);
	
	CTimeLogger::Print("Image fix:");
	
	BuildAndShowBinImage(_mainImage.GrayImage(), true);
	_window.Update(_mainImage.GrayImage());
	
	_mainImage.GrayImage().Save("gray_fixed");
	_mainImage.BinImage().Save("bin_fixed");
}

void CImageProcessor::ProcessFixImageStupid()
{
	std::deque<CImagePatch> patches = FetchPatches({0, 0, MaxPatchSideSize, MaxPatchSideSize});
	std::map<uint64, std::deque<CImagePatch>> clusters = FetchClusters(patches);
	std::deque<uint64> keys = std::deque<uint64>();
	
	for (const auto& it: clusters) {
		uint64 key = it.first;
		keys.push_back(key);
	}
	
	int i = 0;
	// проходим весь диапазон
	for (const uint64& key: keys) {
		// находим нужный нам кластер
		auto cluster = clusters.find(key);
		if (cluster != clusters.end() && !cluster->second.empty()) {
			// сортируем по резкости внутри кластера
			std::deque<CImagePatch> patchCluster = cluster->second;
			std::sort(patchCluster.begin(), patchCluster.end(), LessBlur());
			CImagePatch sharpPatch = patchCluster[0];
			
			// копируем самый резкий патч
			for (const CImagePatch& patch: cluster->second) {
				CImage temp = _mainImage.GrayImage()(patch.GetFrame());
				sharpPatch.GrayImage().copyTo(temp);
				
				temp = _mainImage.BinImage()(patch.GetFrame());
				sharpPatch.BinImage().copyTo(temp);
				
				temp = _mainImage.SdImage()(patch.GetFrame());
				sharpPatch.SdImage().copyTo(temp);
			}
			
			i++;
			std::cout << "Left clusters: " << keys.size() - i << std::endl;
			
			// извлекаем патчи и перекластеризуем
			patches = FetchPatches({0, 0, MaxPatchSideSize, MaxPatchSideSize});
			clusters = FetchClusters(patches);
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


#pragma mark - Utils

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
