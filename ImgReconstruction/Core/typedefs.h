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
    
    TImageCompareMetricNone
} TImageCompareMetric;

typedef enum : int {
	TBlurMeasureMethodStandartDeviation,
	TBlurMeasureMethodDynamicRange,
	TBlurMeasureMethodFFT,
	TBlurMeasureMethodFD,
	
	TBlurMeasureMethodNone
} TBlurMeasureMethod;

typedef enum : int {
    TBrightnessEqualizationMean,
    TBrightnessEqualizationDynRange,
    
    TBrightnessEqualizationNone
} TBrightnessEqualization;

typedef enum : int {
    TCompSumStd,
    TCompSumBorder,
    
    TCompSumNone
} TCompSum;

typedef enum : int {
	TBinarizationMethodNICK,
	TBinarizationMethodNiBlack,
	TBinarizationMethodAdaptiveGaussian,
    
    TBinarizationMethodNone
} TBinarizationMethod;

typedef enum : int {
	TPatchClassifyingMethodAvgHash,
	TPatchClassifyingMethodPHash,
    
    TPatchClassifyingMethodNone
} TPatchClassifyingMethod;

typedef enum : int {
    TPatchFilteringCriteriaContrast = 1,
    TPatchFilteringCriteriaBin = 2,
    TPatchFilteringCriteriaFull = TPatchFilteringCriteriaContrast | TPatchFilteringCriteriaBin,
    
    TPatchFilteringCriteriaNone
} TPatchFilteringCriteria;

typedef enum : int {
    TAccImageSumMethodAvg,
    TAccImageSumMethodMedian,
    
    TAccImageSumMethodNone
} TAccImageSumMethod;