//
//  IMeasurer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 11.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

class IBlurMeasurer : public IImageSubprocessor
{
public:
	virtual ~IBlurMeasurer() {}
	virtual double Measure(const CImage& img) const = 0;
};