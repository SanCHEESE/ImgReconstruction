//
//  IBrightnessEqualizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IImageSubprocessor.h"

#include "CImage.h"

class IBrightnessEqualizer : public IImageSubprocessor
{
public:
	virtual ~IBrightnessEqualizer() {}
	virtual void EqualizeBrightness(CImage& image, const CImage& toImage) const = 0;
	virtual void EqualizeBrightness(cv::cuda::GpuMat& gImage, const cv::cuda::GpuMat& gToImage) const = 0;
};