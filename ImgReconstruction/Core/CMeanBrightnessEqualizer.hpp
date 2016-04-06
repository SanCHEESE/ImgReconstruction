//
//  CMeanBrightnessEqualizer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBrightnessEqualizer.h"

#include "common.h"

class CMeanBrightnessEqualizer : public IBrightnessEqualizer
{
public:
	virtual void EqualizeBrightness(CImage& img1, CImage& img2) const
	{
		img1.convertTo(img1, CV_16S);
		img2.convertTo(img2, CV_16S);

		double mean1 = cv::mean(img1)[0];
		double mean2 = cv::mean(img2)[0];

		double delta = mean1 - mean2;
		CImage deltaMat(img1.rows, img1.cols, CV_16S, cv::Scalar(delta));
		if (delta < 0) {
			// second image is brighter
			img1 += cv::Scalar::all(delta);
		} else {
			// first image is brighter
			img2 += cv::Scalar::all(delta);
		}
	}
};