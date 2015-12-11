//
//  typedefs.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 21.11.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

typedef enum : int {
    TImageCompareMetricL1,
    TImageCompareMetricL2,
} TImageCompareMetric;

typedef enum : int {
    TBlurMeasureMethodStandartDeviation,
    TBlurMeasureMethodDynamicRange,
    TBlurMeasureMethodFFT,
    
    TBlurMeasureMethodNone
} TBlurMeasureMethod;

typedef enum : int {
    TBinarizationMethodNICK,
    TBinarizationMethodNiBlack,
    TBinarizationMethodAdaptiveGaussian,
} TBinarizationMethod;

typedef enum : int {
	TPatchClusteringMethodAvgHash,
	TPatchClusteringMethodPHash,
} TPatchClusteringMethod;
