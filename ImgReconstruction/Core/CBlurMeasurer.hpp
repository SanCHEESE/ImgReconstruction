//
//  CBlurMeasurer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 11.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IMeasurer.hpp"



class CBlurMeasurer: public IMeasurer
{
public:
    CBlurMeasurer(){}
    CBlurMeasurer(TBlurMeasureMethod measureMethod) : _measureMethod(measureMethod) {};
    
    virtual double Measure(const CImage& img) const;
private:
    double MeasureUsingStdDeviation(const CImage& img) const;
    double MeasureUsingDynamicRange(const CImage& img) const;
    double MeasureUsingFFT(const CImage& img) const;
    
    TBlurMeasureMethod _measureMethod;
};