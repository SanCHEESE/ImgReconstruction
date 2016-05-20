//
//  CStdImageSummator.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IImageSummator.h"

class CStdImageSummator : public IImageSummator
{
public:
	virtual float Sum(const CImage& img) const
	{
		return ;
	}
};