//
//  CNICKBinarizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CBinarizer.hpp"

class CNICKBinarizer: public CBinarizer
{
public:
	using CBinarizer::CBinarizer;
	
	virtual CImage Binarize(const CImage& img) const
	{
		std::vector<CImage> imgPatches;
		if (img.rows == _patchSize.height && img.cols == _patchSize.width) {
			imgPatches.push_back(img);
		} else {
			imgPatches = img.GetAllPatches<int>(_patchSize, cv::Point(_patchSize.width, _patchSize.height));
		}
		auto binarizedPatches = std::vector<CImage>(imgPatches.size());
		for (int i = 0; i < imgPatches.size(); i++) {
			CImage patch = imgPatches[i];
			patch.convertTo(patch, CV_64F);
			
			float mean = cv::mean(patch)[0];
			float pixelsSumOfSquares = cv::sum(patch.mul(patch))[0];
			float thresholdValue = mean + _k * sqrt((pixelsSumOfSquares - mean)/patch.GetFrame().area()) + _offset;
			
			CImage binarizedPatch;
			patch.convertTo(patch, CV_8U);
			cv::threshold(patch, binarizedPatch, thresholdValue, 255, cv::THRESH_BINARY);
			
			patch.CopyMetadataTo(binarizedPatch);
			binarizedPatches[i] = binarizedPatch;
		}
		
		CImage resultImg = CImage(img.rows, img.cols, CV_8U, cv::Scalar(0));
		for (const CImage& binarizedPatch: binarizedPatches) {
			CImage tmp = resultImg(cv::Rect(binarizedPatch.GetFrame()));
			binarizedPatch.copyTo(tmp);
		}
		
		return resultImg;
	}
};