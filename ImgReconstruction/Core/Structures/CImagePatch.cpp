//
//  CImagePatch.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 24.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImagePatch.hpp"


#pragma mark - Public

int CImagePatch::ImgClass()
{
    if (_imgClass < 0) {
        _imgClass = CalculateImgClass();
    }
    
    return _imgClass;
}

double CImagePatch::BlurValue(TBlurMeasureMethod method)
{
    if (_blurValue < 0) {
        _blurValue = CalculateBlurValue(method);
    }
    
    return _blurValue;
}

double CImagePatch::StandartDeviation()
{
    if (_standartDeviation < 0) {
        _standartDeviation = CalculateStandartDeviation();
    }
    
    return _standartDeviation;
}


#pragma mark - Private

void CImagePatch::Initialize()
{
    _imgClass = -1;
    _blurValue = -1;
    _standartDeviation = -1;
}

double CImagePatch::CalculateBlurValue(TBlurMeasureMethod method) const
{
    CBlurMeasurer measurer(method);
    return measurer.Measure(_grayImage);
}

double CImagePatch::CalculateStandartDeviation() const
{
    CBlurMeasurer measurer(TBlurMeasureMethodStandartDeviation);
    return measurer.Measure(_grayImage) * 255;
}

int CImagePatch::CalculateImgClass() const
{
    return CImageClassifier::Classify(_grayImage);
}