//
//  CDynRangeBrightnessEqualizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBrightnessEqualizer.h"

class CDynRangeBrightnessEqualizer : public IBrightnessEqualizer
{
public:
    virtual void EqualizeBrightness(CImage& img1, CImage& img2) const
    {
        double min1, max1;
        cv::minMaxLoc(img1, &min1, &max1);
        
        double min2, max2;
        cv::minMaxLoc(img2, &min2, &max2);
        
        // https://en.wikipedia.org/wiki/Normalization_(image_processing)
        bool isFirstImage = true;
        CImage I = img1;
        double min = min1;
        double max = max1;
        double newMin = min2;
        double newMax = max2;
        if (max1 - min1 > max2 - min2) {
            // first range is bigger than second
            I = img2;
            
            min = min2;
            max = max2;
            newMin = min1;
            newMax = max1;
            isFirstImage = false;
        }
        
        for (int i = 0; i < I.rows; i++) {
            for (int j = 0; j < I.cols; j++) {
                I.at<uchar>(i, j) = (I.at<uchar>(i, j) - min) * (newMax - newMin)/(max - min) + newMin;
            }
        }
        
        if (isFirstImage) {
            img1 = I;
        } else {
            img2 = I;
        }
    }
};