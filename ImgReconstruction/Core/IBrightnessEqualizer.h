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
	virtual void EqualizeBrightness(CImage& image, CImage& toImage) const = 0;
};