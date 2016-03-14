//
//  CImageExtender.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IImageExtender.h"

class CImageExtender: public IImageExtender
{
public:
    CImageExtender(const cv::Size& patchSize) : _patchSize(patchSize) {};
    virtual CImage Extent(const CImage& img) const
    {
        cv::Size iterImageSize = {0, 0};
        if (img.cols % _patchSize.width > 0) {
            iterImageSize.width = (img.cols / _patchSize.width + 1) * _patchSize.width;
        } else {
            iterImageSize.width = img.cols;
        }
        
        if (img.rows % _patchSize.height > 0) {
            iterImageSize.height = (img.rows / _patchSize.height + 1) * _patchSize.height;
        } else {
            iterImageSize.height = img.rows;
        }
        
        int avgColor = cv::mean(img)[0] - 5;
        CImage result = CImage(iterImageSize, cv::DataType<uchar>::type, avgColor);
        CImage roi = result(cv::Rect(0, 0, img.cols, img.rows));
        img.copyTo(roi);
        return result;
    }
private:
    cv::Size _patchSize;
};