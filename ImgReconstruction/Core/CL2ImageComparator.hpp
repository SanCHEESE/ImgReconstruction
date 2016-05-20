//
//  CL2ImageComparator.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CImageComparator.hpp"

class CL2ImageComparator : public CImageComparator
{
public:
	using CImageComparator::CImageComparator;

	virtual bool Equal(const CImage& img1, const CImage& img2) const
	{
		CImage normImg1;
		img1.copyTo(normImg1);

		CImage normImg2;
		img2.copyTo(normImg2);

		_equalizer->EqualizeBrightness(normImg1, normImg2);

		cv::Mat result;
		cv::absdiff(normImg1, normImg2, result);
		result.convertTo(result, CV_32S);
		result = result.mul(result);
		float dist = cv::sum(result)[0];
		return dist < _eps;
	}
};
