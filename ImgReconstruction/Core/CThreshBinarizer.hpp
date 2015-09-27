//
//  ThreshBinarizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBinarizer.hpp"

class CThreshBinarizer
{
    
public:
    CThreshBinarizer();
    CThreshBinarizer(double thresholdValue, double maxThresholdBinaryValue = 255., int thresholdType = cv::THRESH_BINARY)
        : _thresholdValue(thresholdValue), _maxThresholdBinaryValue(maxThresholdBinaryValue), _thresholdType(thresholdType){}
    
    
    virtual CImage Binarize(CImage& img);
    
private:
    double _thresholdValue;
    double _maxThresholdBinaryValue;
    int _thresholdType;
};

