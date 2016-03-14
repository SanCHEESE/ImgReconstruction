//
//  CImageSubprocessorHolder.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CImageSubprocessorHolder.h"

#include "CPatchFilter.hpp"
#include "CPatchFetcher.hpp"

#include "CPHashPatchClassifier.hpp"
#include "CAvgHashPatchClassifier.hpp"

#include "CStdDeviationBlurMeasurer.hpp"
#include "CDynamicRangeBlurMeasurer.hpp"
#include "CFFTBlurMeasurer.hpp"
#include "CFDBlurMeasurer.hpp"

#include "CMeanBrightnessEqualizer.hpp"
#include "CDynRangeBrightnessEqualizer.hpp"

#include "CStdImageSummator.hpp"
#include "CBorderImageSummator.hpp"

#include "CL1ImageComparator.hpp"
#include "CL2ImageComparator.hpp"

#include "CAdaptiveGaussianBinarizer.hpp"
#include "CNICKBinarizer.hpp"
#include "CNiBlackBinarizer.hpp"

#include "CImageExtender.hpp"

CBinarizer* Binarizer(TBinarizationMethod method, const cv::Size& patchSize, double k)
{
    CBinarizer *binarizer = 0;
    switch (method) {
        case TBinarizationMethodNICK:
            binarizer = new CNICKBinarizer(patchSize, k);
            break;
        case TBinarizationMethodNiBlack:
            binarizer = new CNiBlackBinarizer(patchSize, k);
            break;
        case TBinarizationMethodAdaptiveGaussian:
            binarizer = new CAdaptiveGaussianBinarizer(patchSize, k);
            break;
        default:
            break;
    }
    return binarizer;
}

IImageSummator* Summator(TCompSum method, double weight)
{
    IImageSummator *summator = 0;
    switch (method) {
        case TCompSumStd:
            summator = new CStdImageSummator();
            break;
        case TCompSumBorder:
            summator = new CBorderImageSummator(weight);
            break;
        default:
            break;
    }
    return summator;
}

IBrightnessEqualizer* BrightnessEqualizer(TBrightnessEqualization equalizationType)
{
    IBrightnessEqualizer* equalizer = 0;
    switch (equalizationType) {
        case TBrightnessEqualizationDynRange:
            equalizer = new CDynRangeBrightnessEqualizer();
            break;
        case TBrightnessEqualizationMean:
            equalizer = new CMeanBrightnessEqualizer();
            break;
        default:
            break;
    }
    
    return equalizer;
}

IImageComparator* Comparator(TImageCompareMetric metric, IBrightnessEqualizer* equalizer, IImageSummator* summator, int eps)
{
    IImageComparator* comparator = 0;
    switch (metric) {
        case TImageCompareMetricL1:
            comparator = new CL1ImageComparator(equalizer, summator, eps);
            break;
        case TImageCompareMetricL2:
            comparator = new CL2ImageComparator(equalizer, summator, eps);
            break;
        default:
            break;
    }
    return comparator;
}

IPatchClassifier* Classifier(TPatchClassifyingMethod method)
{
    IPatchClassifier* classifier = 0;
    switch (method) {
        case TPatchClassifyingMethodPHash:
            classifier = new CPHashPatchClassifier();
            break;
        case TPatchClassifyingMethodAvgHash:
            classifier = new CAvgHashPatchClassifier();
            break;
        default:
            break;
    }
    return classifier;
}

IBlurMeasurer* Measurer(TBlurMeasureMethod method, double param)
{
    IBlurMeasurer* measurer = 0;
    switch (method) {
        case TBlurMeasureMethodFD:
            measurer = new CFDBlurMeasurer();
            break;
        case TBlurMeasureMethodFFT:
            measurer = new CFFTBlurMeasurer(param);
            break;
        case TBlurMeasureMethodDynamicRange:
            measurer = new CDynamicRangeBlurMeasurer();
            break;
        case TBlurMeasureMethodStandartDeviation:
            measurer = new CStdDeviationBlurMeasurer();
            break;
        default:
            break;
    }
    return measurer;
}

CImageSubprocessorHolder::CImageSubprocessorHolder()
{
    IPatchFetcher *patchFetcher = new CPatchFetcher(cv::Size(DefaultMaxPatchSideSize, DefaultMaxPatchSideSize), DefaultPatchOffset);
    _subprocessors[PatchFetcherKey] = patchFetcher;
    
    IBinarizer *filterBinarizer = Binarizer(DefaultBinMethod, DefaultFilteringPatchSize, 1.0);
    _subprocessors[FilterBinarizerKey] = filterBinarizer;
    IPatchFilter *patchFilter = new CPatchFilter(filterBinarizer, DefaultMinPatchContrastValue, DefaultFilteringPatchSize);
    _subprocessors[PatchFilterKey] = patchFilter;
    
    IImageSummator *summator = Summator(DefaultCompSum, DefaultBorderSumWeight);
    _subprocessors[CompImgSummatorKey] = summator;
    IBrightnessEqualizer *brightnessEqualizer = BrightnessEqualizer(DefaultBrightnessEqualization);
    _subprocessors[CompBrightnessEqualizerKey] = brightnessEqualizer;
    IImageComparator* comparator = Comparator(DefaultCompMetric, brightnessEqualizer, summator, DefaultComparisonEps);
    _subprocessors[ComparatorKey] = comparator;
    
    IBinarizer *binarizer = Binarizer(DefaultBinMethod, DefaultBinPatchSize, DefautBinK);
    _subprocessors[BinarizerKey] = binarizer;
    
    IPatchClassifier* classifier = Classifier(DefaultClassifyingMethod);
    _subprocessors[PatchClassifierKey] = classifier;
    
    IBlurMeasurer* measurer = Measurer(DefaultBlurMeasureMethod, DefaultBlurMetricRadiusRatio);
    _subprocessors[BlurMeasurerKey] = measurer;
    
    IImageExtender* extender = new CImageExtender(DefaultBinPatchSize);
    _subprocessors[ImageExtenderKey] = extender;
    
    _config.accImageSumMethod = DefaultAccImageSumMethod;
};

CImageSubprocessorHolder::~CImageSubprocessorHolder()
{
    this->Reset();
}

void CImageSubprocessorHolder::Reset()
{
    for (auto& subprocessor: _subprocessors) {
        delete subprocessor.second;
    }
}
