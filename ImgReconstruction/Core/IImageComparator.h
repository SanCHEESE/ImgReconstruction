//
//  IImageComparator.h
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
    virtual double operator()(const CImagePatch& patch1, const CImagePatch& patch2) const = 0;
};
