//
//  config.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 21.11.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "typedefs.h"

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
static const std::string PatchOffsetJsonKey = "offset";
static const std::string PatchSizeJsonKey = "size";
static const std::string SummatorWeightJsonKey = "weight";
static const std::string ContrastValueJsonKey = "contrast";
static const std::string BlurRatioJsonKey = "ratio";

static const std::string BlurMeasureJsonKey = "blur";
static const std::string ExtenderPatchSizeJsonKey = "extend";
static const std::string AccSumJsonKey = "acc";
static const std::string ClassifierJsonKey = "classifier";
static const std::string PatchFilterJsonKey = "filter";
static const std::string PatchFetchJsonKey = "fetch";
static const std::string ImageSumJsonKey = "sum";
static const std::string BrightnessEqualizeJsonKey = "equalize";
static const std::string ImageCompareJsonKey = "compare";

// patch fetching
static cv::Point DefaultPatchOffset = {1, 1};
static int DefaultMaxPatchSideSize = 8;

// comparison
static TImageCompareMetric DefaultCompMetric = TImageCompareMetricL1;
static int DefaultComparisonEps = 1000;
static TBrightnessEqualization DefaultBrightnessEqualization = TBrightnessEqualizationDynRange;
static TCompSum DefaultCompSum = TCompSumStd;
static double DefaultBorderSumWeight = 1.5;

// blur config
static float DefaultBlurMetricRadiusRatio = 0.4f;
static TBlurMeasureMethod DefaultBlurMeasureMethod = TBlurMeasureMethodFD;

// classification method
static TPatchClassifyingMethod DefaultClassifyingMethod = TPatchClassifyingMethodPHash;

// binarization
static TBinarizationMethod DefaultBinMethod = TBinarizationMethodNiBlack;
static cv::Size DefaultBinPatchSize = {25, 25};
static double DefautBinK = -0.2;

// patch filtering
static cv::Size DefaultFilteringPatchSize = {2, 2};
static double DefaultFilteringBinK = 1.;
static float DefaultMinPatchContrastValue = 40;

// acc image
static TAccImageSumMethod DefaultAccImageSumMethod = TAccImageSumMethodAvg;


