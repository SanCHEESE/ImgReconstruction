//
//  CDynRangeBrightnessEqualizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include <IBrightnessEqualizer.h>
#include <common.h> 

class CDynRangeBrightnessEqualizer : public IBrightnessEqualizer
{
public:
	virtual inline void EqualizeBrightness(CImage& image, const CImage& toImage) const
	{
		double min1, max1;
		cv::minMaxLoc(image, &min1, &max1);

		double min2, max2;
		cv::minMaxLoc(toImage, &min2, &max2);

		// https://en.wikipedia.org/wiki/Normalization_(image_processing)

		double min = min1;
		double max = max1;
		double newMin = min2;
		double newMax = max2;

		for (int i = 0; i < image.rows; i++) {
			for (int j = 0; j < image.cols; j++) {
				image.at<uchar>(i, j) = (image.at<uchar>(i, j) - min) * (newMax - newMin) / (max - min) + newMin;
			}
		}
	}

	virtual inline void EqualizeBrightness(cuda::GpuMat& gImage, const cuda::GpuMat& gToImage) const
	{	
		double min1, max1;
		cuda::minMax(gImage, &min1, &max1);

		double min2, max2;
		cuda::minMax(gToImage, &min2, &max2);

		double min = min1;
		double max = max1;
		double newMin = min2;
		double newMax = max2;

		cuda::subtract(gImage, min, gImage);
		cuda::multiply(gImage, newMax - newMin, gImage);
		cuda::divide(gImage, max - min, gImage);
		cuda::add(gImage, newMin, gImage);
	}

private:

};