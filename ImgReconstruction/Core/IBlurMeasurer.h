//
//  IMeasurer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 11.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IImageSubprocessor.h"
#include "CImage.h"

#include "common.h"

class IBlurMeasurer : public IImageSubprocessor
{
public:
	virtual ~IBlurMeasurer() {}
	virtual float Measure(const CImage& img, const CImage* const parentImg = 0) const = 0;
};