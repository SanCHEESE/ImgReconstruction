//
//  config.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 21.11.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "typedefs.h"

// сonfigs
static const std::string DebugWindowName = "Debug";
static const std::string BinarizedWindowName = "Binarized";
static const std::string SaveImgPath = "../../out/";

static const std::string PatchOffsetConfigKey = "PatchOffsetConfigKey";
static const std::string MaxPatchSideSizeConfigKey = "MaxPatchSideSizeConfigKey";
static const std::string CompMetricConfigKey = "CompMetricConfigKey";
static const std::string ComparisonEpsL1ConfigKey = "ComparisonEpsL1ConfigKey";
static const std::string ComparisonEpsL2ConfigKey = "ComparisonEpsL2ConfigKey";
static const std::string BlurMetricRadiusRatioConfigKey = "BlurMetricRadiusRatioConfigKey";
static const std::string BlurMeasureMethodConfigKey = "BlurMeasureMethodConfigKey";
static const std::string ClassifyingMethodConfigKey = "ClassifyingMethodConfigKey";
static const std::string BinMethodConfigKey = "BinMethodConfigKey";
static const std::string BinaryWindowSizeConfigKey = "BinaryWindowSizeConfigKey";
static const std::string FilteringPatchSizeConfigKey = "FilteringPatchSizeConfigKey";
static const std::string MinPatchContrastValueConfigKey = "MinPatchContrastValueConfigKey";
static const std::string PatchFileringCriteriaConfigKey = "PatchFileringCriteriaConfigKey";
static const std::string AccImageSumMethodConfigKey = "AccImageSumMethodConfigKey";

// patch fetching
static cv::Point DefaultPatchOffset = {1, 1};
static int DefaultMaxPatchSideSize = 8;

// comparison
static TImageCompareMetric DefaultCompMetric = TImageCompareMetricL1;
static int DefaultComparisonEpsL1 = 900;
static int DefaultComparisonEpsL2 = 2000;

// blur config
static float DefaultBlurMetricRadiusRatio = 0.4f;
static TBlurMeasureMethod DefaultBlurMeasureMethod = TBlurMeasureMethodFD;

// classification method
static TPatchClassifyingMethod DefaultClassifyingMethod = TPatchClassifyingMethodPHash;

// binarization
static TBinarizationMethod DefaultBinMethod = TBinarizationMethodNICK;
static cv::Size DefaultBinaryWindowSize = {25, 25};

// patch filtering
static cv::Size DefaultFilteringPatchSize = {2, 2};
static float DefaultMinPatchContrastValue = 30;
static TPatchFilteringCriteria DefaultPatchFileringCriteria = TPatchFilteringCriteriaFull;

// acc image
static TAccImageSumMethod DefaultAccImageSumMethod = TAccImageSumMethodAvg;


