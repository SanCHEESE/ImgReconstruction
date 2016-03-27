//
//  CAdaptiveGaussianBinarizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CBinarizer.hpp"

class CAdaptiveGaussianBinarizer: public CBinarizer
{
public:
    using CBinarizer::CBinarizer;
    
    virtual CImage Binarize(const CImage& img) const
    {
        CImage resultImg;
        cv::adaptiveThreshold(img, resultImg, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, _patchSize.width, _k);
        return resultImg;
    }
};