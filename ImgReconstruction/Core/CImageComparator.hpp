//
//  IImageComparator.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//
#pragma once

#include "IBrightnessEqualizer.h"

class CImageComparator : public IImageComparator
{
public:
	CImageComparator(IBrightnessEqualizer* equalizer, int eps) : _equalizer(equalizer), _eps(eps) {};

	virtual bool Equal(const CImagePatch& patch1, const CImagePatch& patch2) const
	{
		return Equal(patch1.GrayImage(), patch2.GrayImage());
	}
	virtual bool Equal(const CImage& img1, const CImage& img2) const = 0;
	virtual bool Equal(const cv::cuda::GpuMat& gImg1, const cv::cuda::GpuMat& gImg2) = 0;
protected:
	IBrightnessEqualizer* _equalizer;
	int _eps;

	cuda::GpuMat _gTemp;
};
