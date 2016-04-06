//
//  CDynamicRangeBlurMeasurer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBlurMeasurer.h"

class CDynamicRangeBlurMeasurer : public IBlurMeasurer
{
public:
	virtual double Measure(const CImage& img) const
	{
		//std::cout << img << std::endl;

		//int histSize = 256;
		//float range[] = {0, 256} ;
		//const float* histRange = {range};
		//
		//cv::Mat histogram;
		//cv::calcHist( &img, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);
		//
		//std::vector<unsigned char> histArray;
		//histogram.col(0).copyTo(histArray);
		//auto minMaxElem = std::minmax_element(histArray.begin(), histArray.end());

		double min, max;
		cv::minMaxLoc(img, &min, &max);

		//return std::abs(*(minMaxElem.first) - *(minMaxElem.second));
		return std::abs(min - max);
	}
};