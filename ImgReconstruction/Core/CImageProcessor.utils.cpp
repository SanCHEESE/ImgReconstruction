//
//  CImageProcessor.utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include <CImageProcessor.h>
#include <CTimeLogger.h>

#include <annoylib.h>
#include <kissrandom.h>

CImage CreateHistImage(const std::map<int, std::deque<CImagePatch>>& data)
{
	std::map<uint64, std::deque<CImagePatch>> temp;
	for (auto& d : data) {
		temp.insert(d);
	}

	return CreateHistImage(temp);
}

CImage CreateHistImage(const std::map<int, std::unordered_set<CImagePatch, CImagePatch::hasher>>& data)
{
	std::map<uint64, std::unordered_set<CImagePatch, CImagePatch::hasher>> temp;
	for (auto& d : data) {
		temp.insert(d);
	}

	return CreateHistImage(temp);
}

CImage CreateHistImage(const std::map<uint64, std::unordered_set<CImagePatch, CImagePatch::hasher>>& data)
{
	cv::Size patchSize = data.begin()->second.begin()->GetSize();
	int maxHeight = 0;
	for (auto& it : data) {
		int rowHeigt = it.second.size() * (patchSize.height + 1);
		if (maxHeight < rowHeigt) {
			maxHeight = rowHeigt;
		}
	}

	CImage histogramImg(maxHeight + 3 + 50, data.size() * (patchSize.width + 3) + 3, CV_8UC1, cv::Scalar(255));
	int x = 0;
	for (auto& it : data) {
		auto patches = it.second;
		CImage columnImage(1, patchSize.width, CV_8UC1, cv::Scalar(255));

		for (const auto& patch : patches) {
			CImage greyPatchImg = patch.GrayImage();
			cv::Mat horisontalSeparator(1, greyPatchImg.GetFrame().width, CV_8UC1, cv::Scalar(255));
			cv::vconcat(columnImage, greyPatchImg, columnImage);
			cv::vconcat(columnImage, horisontalSeparator, columnImage);
		}

		CImage textImg = CImage::GetImageWithText(std::to_string(patches.size()), cv::Point(1, 10), RGB(0, 0, 0), RGB(255, 255, 255), cv::Size(50, 11));
		textImg = textImg.GetRotatedImage(-90);
		cv::Mat roi = histogramImg.rowRange(histogramImg.rows - 50 - columnImage.rows, histogramImg.rows - 50).colRange(x, x + columnImage.cols);
		columnImage.copyTo(roi);

		cv::Mat roi2 = histogramImg.rowRange(histogramImg.rows - 50, histogramImg.rows).colRange(x, x + textImg.cols);
		textImg.copyTo(roi2);

		x += columnImage.cols + 3;
	}

	return histogramImg;
}

CImage CreateHistImage(const std::map<uint64, std::deque<CImagePatch>>& data)
{
	cv::Size patchSize = data.begin()->second.begin()->GetSize();
	int maxHeight = 0;
	for (auto& it : data) {
		int rowHeigt = it.second.size() * (patchSize.height + 1);
		if (maxHeight < rowHeigt) {
			maxHeight = rowHeigt;
		}
	}

	CImage histogramImg(maxHeight + 3 + 50, data.size() * (patchSize.width + 3) + 3, CV_8UC1, cv::Scalar(255));
	int x = 0;
	for (auto& it : data) {
		auto patches = it.second;
		CImage columnImage(1, patchSize.width, CV_8UC1, cv::Scalar(255));

		for (const auto& patch : patches) {
			CImage greyPatchImg = patch.GrayImage();
			cv::Mat horisontalSeparator(1, greyPatchImg.GetFrame().width, CV_8UC1, cv::Scalar(255));
			cv::vconcat(columnImage, greyPatchImg, columnImage);
			cv::vconcat(columnImage, horisontalSeparator, columnImage);
		}

		CImage textImg = CImage::GetImageWithText(std::to_string(patches.size()), cv::Point(1, 10), RGB(0, 0, 0), RGB(255, 255, 255), cv::Size(50, 11));
		textImg = textImg.GetRotatedImage(-90);
		cv::Mat roi = histogramImg.rowRange(histogramImg.rows - 50 - columnImage.rows, histogramImg.rows - 50).colRange(x, x + columnImage.cols);
		columnImage.copyTo(roi);

		cv::Mat roi2 = histogramImg.rowRange(histogramImg.rows - 50, histogramImg.rows).colRange(x, x + textImg.cols);
		textImg.copyTo(roi2);

		x += columnImage.cols + 3;
	}

	return histogramImg;
}

void* CImageProcessor::GetAnnoyIndexInterfaceForCurrentMetric(size_t size) const
{
	TImageCompareMetric metric = _subprocHolder->GetConfig().cmpMetric;
	AnnoyIndexInterface<int, float>* p = 0;
	switch (metric) {
		case TImageCompareMetricL1:
			p = new AnnoyIndex<int, float, L1, Kiss64Random>(size);
			break;
		case TImageCompareMetricL2:
			p = new AnnoyIndex<int, float, L2, Kiss64Random>(size);
			break;
		case TImageCompareMetricL3:
			p = new AnnoyIndex<int, float, L3, Kiss64Random>(size);
			break;
		default:
			break;
	}
	return (void *)p;
}

std::map<int, std::deque<CImagePatch>> CImageProcessor::Clusterize(std::deque<CImagePatch>& aClass)
{
	IImageComparator* comparator = _subprocHolder->ImageComparator();

	int patchSize = aClass[0].GetSize().area();

	std::map<int, std::deque<CImagePatch>> clusters;
	std::vector<bool> clusterized(aClass.size(), false);

	std::vector<float*> imgs;
	imgs.reserve(aClass.size());

	AnnoyIndexInterface<int, float>* t = (AnnoyIndexInterface<int, float>*)GetAnnoyIndexInterfaceForCurrentMetric(patchSize);
	for (int i = 0; i < aClass.size(); i++) {
		CImage img;
		aClass[i].GrayImage().convertTo(img, CV_32FC1);
		// cast dyn range to 0..255
		double min, max;
		cv::minMaxLoc(img, &min, &max);
		img = (img - min) * 255 / (max - min);

		int size = img.cols * img.rows;
		float *imgArray = new float[size];
		std::memcpy(imgArray, img.data, size * sizeof(float));

		t->add_item(i, imgArray);

		imgs.push_back(imgArray);
	}

	t->build(10);

	for (int i = 0; i < aClass.size(); i++) {
		std::vector<int> indices;
		std::vector<float> distances;
		t->get_nns_by_item(i, aClass.size() - 1, -1, &indices, &distances);
		
		std::deque<CImagePatch> cluster;
		for (int j = 0; j < distances.size(); j++) {
			int idx = indices[j];
			if (comparator->GetEps() > distances[j]) {
				clusterized[idx] = true;
				cluster.push_back(aClass[idx]);
			}
		}
		clusterized[i] = true;

		cluster.push_back(aClass[i]);

		clusters[i] = cluster;
	}

	t->unload();
	delete t;

	for (int i = 0; i < imgs.size(); i++) {
		delete imgs[i];
	}

	return clusters;
}

std::map<int, std::deque<CImagePatch>> CImageProcessor::Clusterize(std::unordered_set<CImagePatch, CImagePatch::hasher>& aClass)
{
	std::map<int, std::deque<CImagePatch>> clusters;

#if ENABLE_CUDA
	cuda::Stream stream;
	// upload all patches to gpu memory
	std::list<cuda::GpuMat> gaClass;
	for (const CImagePatch &patch: aClass) {
		CGpuMat gm;
		gm.upload(patch.GrayImage(), stream);
		gaClass.push_back(gm);
	}
	stream.waitForCompletion();
#endif

	IImageComparator* comparator = _subprocHolder->ImageComparator();
	int aClassIdx = 0;
	//long k = 0;
	while (!aClass.empty()) {
		std::deque<std::unordered_set<CImagePatch, CImagePatch::hasher>::iterator> toRemove;

		std::deque<CImagePatch> similarPatches;

		auto firstPatch = aClass.begin();
		similarPatches.push_back(*firstPatch);
		similarPatches[0].aClass = aClassIdx;

		aClass.erase(firstPatch);

		//CTimeLogger::StartLogging();

#if ENABLE_CUDA
		cuda::GpuMat gm = *gaClass.begin();
		gaClass.erase(gaClass.begin());
		auto git = gaClass.begin();
		std::deque<std::list<cuda::GpuMat>::iterator> gToRemove;
	
		for (auto it = aClass.begin(); it != aClass.end(); it++, git++) {
#else
		for (auto it = aClass.begin(); it != aClass.end(); it++) {
#endif

#if ENABLE_CUDA
			if (comparator->Equal(gm, *git)) {
#else
			if (comparator->Equal(similarPatches[0], *it)) {
#endif
				similarPatches.push_back(*it);
				similarPatches[similarPatches.size() - 1].aClass = aClassIdx;
				toRemove.push_back(it);
#if ENABLE_CUDA
				gToRemove.push_back(git);
#endif
			}
		}

		//CTimeLogger::PrintTime(std::to_string(aClass.size()));


		// clean up used
		for (auto& r: toRemove) {
			aClass.erase(r);
		}

#if ENABLE_CUDA
		for (auto& r : gToRemove) {
			gaClass.erase(r);
		}
#endif

		clusters[aClassIdx] = similarPatches;
		aClassIdx++;

		_patchesProcessed += toRemove.size() + 1;
		std::ofstream out;
		out.open(_progressFile);
		out << "Progress: " << std::setprecision(3) << (float)_patchesProcessed / _totalPatches * 100 << "% " << _patchesProcessed << " of " << _totalPatches << "\n";
		out << "Last iter clusterized = " << toRemove.size() + 1 <<  " eps = " << comparator->GetEps() << "\n";
		out.close();
	}
	return clusters;
}