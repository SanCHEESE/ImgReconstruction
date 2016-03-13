//
//  CDocumentBinarizer.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 10.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CDocumentBinarizer.h"
#include "CImageProcessor.h"

static double const k = -0.2f;

CImage CDocumentBinarizer::Binarize(const CImage &img) const
{
	CImage resultImg;
	
	if (_binMethod == TBinarizationMethodAdaptiveGaussian) {
		cv::adaptiveThreshold(img, resultImg, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, _patchSize.width, _c);
	} else {
		auto imgPatches = img.GetAllPatches(_patchSize, cv::Point(_patchSize.width, _patchSize.height));
		auto binarizedPatches = std::vector<CImage>(imgPatches.size());
		for (int i = 0; i < imgPatches.size(); i++) {
            
			CImage patch = imgPatches[i];
			patch.convertTo(patch, CV_64F);
			
			double mean = cv::mean(patch)[0];
			
			double thresholdValue;
			switch (_binMethod) {
				case TBinarizationMethodNICK:
				{
					double pixelsSumOfSquares = cv::sum(patch.mul(patch))[0];
					thresholdValue = mean + k * sqrt((pixelsSumOfSquares - mean)/patch.GetFrame().area());
					break;
				}
				case TBinarizationMethodNiBlack:
					thresholdValue = mean + k * utils::StandartDeviation(img) - 10;
					break;
				default:
					break;
			}
			
			CImage binarizedPatch;
			patch.convertTo(patch, CV_8U);
			cv::threshold(patch, binarizedPatch, thresholdValue, 255, cv::THRESH_BINARY);
			
			patch.CopyMetadataTo(binarizedPatch);
			binarizedPatches[i] = binarizedPatch;
		}
		
		resultImg = CImage(img.rows, img.cols, CV_8U, cv::Scalar(0));
		for (const CImage& binarizedPatch: binarizedPatches) {
			CImage tmp = resultImg(cv::Rect(binarizedPatch.GetFrame()));
			binarizedPatch.copyTo(tmp);
		}
	}
	
	return resultImg;
}