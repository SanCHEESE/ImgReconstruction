//
//  config.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 21.11.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "common.h"

// common
static const std::string XJsonKey = "x";
static const std::string YJsonKey = "y";
static const std::string WidthJsonKey = "width";
static const std::string HeightJsonKey = "height";
static const std::string KJsonKey = "k";

static const std::string BinJsonKey = "bin";
static const std::string MethodJsonKey = "method";
static const std::string MetricJsonKey = "metric";
static const std::string EpsJsonKey = "eps";
static const std::string OffsetJsonKey = "offset";
static const std::string SizeJsonKey = "size";
static const std::string WeightJsonKey = "weight";
static const std::string ContrastJsonKey = "contrast";
static const std::string SumJsonKey = "sum";
static const std::string ExtentJsonKey = "extent";
static const std::string CutJsonKey = "cut";
static const std::string CalcJsonKey = "calc";

static const std::string BlurJsonKey = "blur";
static const std::string ClassifierJsonKey = "classifier";
static const std::string FilterJsonKey = "filter";
static const std::string FetchJsonKey = "fetch";
static const std::string CompareJsonKey = "compare";

static const std::string ThreshJsonKey = "thresh";

static const std::string KernelJsonKey = "kernel";
static const std::string TypeJsonKey = "type";
static const std::string AJsonKey = "a";
static const std::string BJsonKey = "b";
static const std::string CJsonKey = "c";

// patch fetching
static cv::Point DefaultPatchOffset = {1, 1};
static int DefaultMaxPatchSideSize = 8;

// comparison
static TImageCompareMetric DefaultCompMetric = TImageCompareMetricL1;
static int DefaultComparisonEps = 1000;

// blur config
static TBlurMeasureMethod DefaultBlurMeasureMethod = TBlurMeasureMethodDynamicRange;

// classification method
static TPatchClassifyingMethod DefaultClassifyingMethod = TPatchClassifyingMethodPHash;

// binarization
static TBinarizationMethod DefaultBinMethod = TBinarizationMethodNiBlack;
static cv::Size DefaultBinPatchSize = {25, 25};
static float DefautBinK = -0.2f;
static float DefaultThreshOffset = -10.f;

// patch filtering
static cv::Size DefaultFilteringPatchSize = {2, 2};
static float DefaultFilteringBinK = 1.;
static float DefaultMinPatchContrastValue = 40;


