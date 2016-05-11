//
//  CBinarizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once 

#include "IBinarizer.h"

class CBinarizer: public IBinarizer
{
public:
	CBinarizer(const cv::Size& patchSize, float k, float offset = 0) : _patchSize(patchSize), _k(k), _offset(offset) {};
	virtual CImage Binarize(const CImage& img) const = 0;
protected:
	cv::Size _patchSize;
	float _k;
	float _offset;
};