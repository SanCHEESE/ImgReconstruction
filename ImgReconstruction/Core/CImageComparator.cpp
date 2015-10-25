//
//  CImageComparator.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageComparator.hpp"

double CImageComparator::Compare(const CImage& img1, const CImage& img2) const
{
    CImage result;
    cv::absdiff(img1, img2, result);
    return sum(result)[0] / 255;
}