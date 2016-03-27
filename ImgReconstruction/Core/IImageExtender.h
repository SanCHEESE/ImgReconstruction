//
//  IImageExtender.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CImage.h"

class IImageExtender: public IImageSubprocessor
{
public:
    virtual ~IImageExtender() {}
    virtual CImage Extent(const CImage& img) const = 0;
};