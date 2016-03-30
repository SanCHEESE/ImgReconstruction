//
//  CFFTBlurMeasurer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBlurMeasurer.h"

class CFFTBlurMeasurer: public IBlurMeasurer
{
public:
    CFFTBlurMeasurer(double radiusRatio) : _radiusRatio(radiusRatio) {};
    
    virtual double Measure(const CImage& img) const
    {
        CImage fft = img.GetFFTImage();
        img.CopyMetadataTo(fft);
        
        cv::Size submatrixSize = cv::Size(ceil(img.cols * _radiusRatio), ceil(img.rows * _radiusRatio));
        cv::Point submatrixOrigin = cv::Point((img.cols - submatrixSize.width) / 2, (img.rows - submatrixSize.height) / 2);
        cv::Rect submatrixRect = cv::Rect(submatrixOrigin, submatrixSize);
        
        CImage imageCopy;
        img.copyTo(imageCopy);
        
        CImage roi = imageCopy(submatrixRect);
        roi.setTo(0);
        
        return -cv::sum(imageCopy)[0];
    }
    
private:
    double _radiusRatio;
};