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

		_equalizer->EqualizeBrightness(normImg1, img2);

		double sum = 0;
		for (int i = 0; i < normImg1.rows; i++) {
			for (int j = 0; j < normImg1.cols; j++) {
				sum += std::abs(normImg1.at<uchar>(i, j) - img2.at<uchar>(i, j));

				if (sum > _eps) break;
			}
			if (sum > _eps) break;
		}

		//CImage result;
		//cv::absdiff(normImg1, normImg2, result);

		//float sum = cv::sum(result)[0];

		//double sum = cv::norm(normImg1, normImg2, cv::NORM_L1);

		return sum < _eps;
	}

	virtual bool Equal(const cv::cuda::GpuMat& gImg1, const cv::cuda::GpuMat& gImg2)
	{
		if (_gTemp.cols == 0 && _gTemp.rows == 0) {
			_gTemp = cuda::GpuMat(gImg1.rows, gImg1.cols, CV_32S, cv::Scalar(0));
		}

		gImg1.copyTo(_gTemp);

		_equalizer->EqualizeBrightness(_gTemp, gImg2);

		cuda::absdiff(_gTemp, gImg2, _gTemp);
		return cuda::sum(_gTemp)[0] < _eps;
	}
};
