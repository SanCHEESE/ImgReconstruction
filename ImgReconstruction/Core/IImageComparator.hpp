//
//  IImageComparator.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//
#pragma once

class IImageComparator
{
public:
    ~IImageComparator() {}
    virtual double Compare(const CImage& img1, const CImage& img2) const = 0;
};
