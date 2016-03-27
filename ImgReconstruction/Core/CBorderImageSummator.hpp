//
//  CBorderImageSummator.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IImageSummator.h"

class CBorderImageSummator: public IImageSummator
{
public:
    CBorderImageSummator(double borderWeight) : _borderWeight(borderWeight) {}
    
    virtual double Sum(const CImage& img) const
    {
        double sum = 0;
        for (int i = 0; i < img.rows; i++) {
            for (int j = 0; j < img.cols; j++) {
                bool isBorderPixel = i == 0 || j == 0 || (i == img.rows - 1) || (j == img.cols - 1);
                sum += isBorderPixel ? (_borderWeight * img.at<uchar>(i, j)) : img.at<uchar>(i, j);
            }
        }
        return sum;
    }
    
private:
    double _borderWeight;
};