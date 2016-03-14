//
//  IImageSummator.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

class IImageSummator : public IImageSubprocessor
{
public:
    virtual ~IImageSummator() {}
    virtual double Sum(const CImage& img) const = 0;
};