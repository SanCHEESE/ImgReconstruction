//
//  CImageComparatorL1.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageComparatorL1.hpp"

double CImageComparatorL1::Compare(const CImage& img1, const CImage& img2)
{
    return cv::sum(img1 - img2)[0];
}