//
//  CBinarizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once 

#include "IBinarizer.h"

class CBinarizer: public IBinarizer
{
public:
    CBinarizer(const cv::Size& patchSize, double k) : _patchSize(patchSize), _k(k) {};
    virtual CImage Binarize(const CImage& img) const = 0;
protected:
    cv::Size _patchSize;
    double _k;
};