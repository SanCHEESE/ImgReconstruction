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
	virtual float Measure(const CImage& img) const
	{
		double min, max;
		cv::minMaxLoc(img, &min, &max);
		return (float)std::abs(min - max);
	}
};