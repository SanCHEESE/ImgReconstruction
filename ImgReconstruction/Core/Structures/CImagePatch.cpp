//
//  CImagePatch.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 24.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImagePatch.hpp"
#include "CImageProcessor.hpp"

#pragma mark - Public

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

int64 CImagePatch::PHash()
{
    if (!_pHashComputed) {
        _pHash = utils::PHash(_grayImage);
        _avgHashComputed = true;
    }
    return _pHash;
}

int64 CImagePatch::AvgHash()
{
    if (!_avgHashComputed) {
        _avgHashComputed = utils::AvgHash(_grayImage);
        _avgHashComputed = true;
    }
    return _avgHash;
}

std::ostream& operator<<(std::ostream& os, const CImagePatch& patch)
{
    os << "Patch:\n";
    os << "\tFrame:\n\t\t" << patch.GetFrame() << std::endl;
    os << "\tBlur value:\n\t\t" << patch.GetBlurValue() << std::endl;
    os << "\tStandart deviation:\n\t\t" << patch.GetStandartDeviation() << std::endl;
    
    os << "\tGrey image:\n" << patch.GrayImage() << std::endl;
    os << "\tBin image:\n" << patch.BinImage() << std::endl;
    os << "\tSd image:\n" << patch.SdImage() << std::endl;
    
    std::bitset<sizeof(int64)> phash(patch.GetPHash());
    os << "\tPHash:\n\t\t" << phash << std::endl;
    
    std::bitset<sizeof(int64)> avgHash(patch.GetAvgHash());
    os << "\tAvgHash:\n\t\t" << avgHash << std::endl;
    
    return os;
}

#pragma mark - Private

void CImagePatch::Initialize()
{
    _imgClass = -1;
    _blurValue = -1;
    _standartDeviation = -1;
    _pHashComputed = false;
    _avgHashComputed = false;
}

double CImagePatch::CalculateBlurValue(TBlurMeasureMethod method) const
{
    CBlurMeasurer measurer(method);
    return measurer.Measure(_grayImage);
}

double CImagePatch::CalculateStandartDeviation() const
{
    return utils::StandartDeviation(_grayImage);
}