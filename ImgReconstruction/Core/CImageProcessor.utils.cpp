//
//  CImageProcessor.utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.h"
#include "CTimeLogger.h"

CImage CreateHistImage(const std::map<int, std::vector<CImagePatch>>& data)
{
	std::map<uint64, std::vector<CImagePatch>> temp;
	for (auto& d : data) {
		temp.insert(d);
	}

	return CreateHistImage(temp);
}

CImage CreateHistImage(const std::map<uint64, std::vector<CImagePatch>>& data)
{
	cv::Size patchSize = data.begin()->second[0].GetSize();
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
		for (int i = 0; i < patches.size(); i++) {
			CImage greyPatchImg = patches[i].GrayImage();
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

std::map<int, std::vector<CImagePatch>> CImageProcessor::Clusterize(std::vector<CImagePatch> aClass)
{
	std::map<int, std::vector<CImagePatch>> clusters;

	cuda::Stream stream;
	// upload all patches to gpu memory
	std::vector<cv::cuda::GpuMat> gaClass;
	for (const CImagePatch &patch: aClass) {
		cv::cuda::GpuMat gm;
		gm.upload(patch.GrayImage(), stream);
		gaClass.push_back(gm);
	}
	stream.waitForCompletion();

	IImageComparator* comparator = _subprocHolder->ImageComparator();
	int aClassIdx = 0;
	for (int i = 0; i < aClass.size(); i++) {

		std::vector<CImagePatch> similarPatches;
		similarPatches.push_back(aClass[i]);
		aClass[i].aClass = aClassIdx;

		for (int j = 1; j < aClass.size(); j++) {
			if (comparator->Equal(gaClass[i], gaClass[j])) {
				similarPatches.push_back(aClass[j]);
				aClass[j].aClass = aClassIdx;
				aClass.erase(aClass.begin() + j);
				gaClass.erase(gaClass.begin() + j);
				j--;
			}
		}

		gaClass.erase(gaClass.begin());
		aClass.erase(aClass.begin());
		clusters[aClassIdx] = similarPatches;

		aClassIdx++;
		i--;
	}

	return clusters;
}