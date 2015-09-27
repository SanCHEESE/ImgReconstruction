//
//  CThreshBinarizer.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CThreshBinarizer.hpp"

CImage CThreshBinarizer::Binarize(CImage& img)
{
    CImage result = CImage();
    cv::threshold(img, result, _thresholdValue, _maxThresholdBinaryValue, _thresholdType);
    return result;
}