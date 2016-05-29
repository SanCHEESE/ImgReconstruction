//
//  CL1ImageComparator.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CImageComparator.hpp"

class CL1ImageComparator : public CImageComparator
{
public:
	using CImageComparator::CImageComparator;

	CL1ImageComparator(IBrightnessEqualizer* equalizer, int eps) : CImageComparator(equalizer, eps) {};

	virtual bool Equal(const CImage& img1, const CImage& img2) const
	{
		CImage normImg1;
		img1.copyTo(normImg1);

		CImage normImg2;
		img2.copyTo(normImg2);

		_equalizer->EqualizeBrightness(normImg1, normImg2);

		CImage result;
		cv::absdiff(normImg1, normImg2, result);

		float sum = cv::sum(result)[0];

		return sum < _eps;
	}
};
