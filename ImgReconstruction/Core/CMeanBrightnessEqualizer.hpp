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
	virtual void EqualizeBrightness(CImage& image, CImage& toImage) const
	{
		image.convertTo(image, CV_16S);
		toImage.convertTo(toImage, CV_16S);

		double mean1 = cv::mean(image)[0];
		double mean2 = cv::mean(toImage)[0];

		double delta = mean1 - mean2;
		image += cv::Scalar::all(delta);
	}
};