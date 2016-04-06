//
//  IImageComparator.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//
#pragma once

#include "IBrightnessEqualizer.h"
#include "IImageSummator.h"

class CImageComparator : public IImageComparator
{
public:
	CImageComparator(IBrightnessEqualizer* equalizer, IImageSummator* summator, int eps) : _equalizer(equalizer), _summator(summator), _eps(eps) {};
protected:
	IBrightnessEqualizer* _equalizer;
	IImageSummator* _summator;
	int _eps;
};
