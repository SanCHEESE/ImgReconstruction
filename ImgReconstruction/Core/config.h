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

// patch fetching
static cv::Point PatchOffset = {1, 1};
static int MaxPatchSideSize = 8;

// comparison
static TImageCompareMetric CompMetric = TImageCompareMetricL1;
static double ComparisonEpsL1 = 900;
static double ComparisonEpsL2 = 2000;

// blur config
static float BlurMetricRadiusRatio = 0.4f;
static TBlurMeasureMethod BlurMeasureMethod = TBlurMeasureMethodFD;

// classification method
static TPatchClassifyingMethod ClassifyingMethod = TPatchClassifyingMethodPHash;

// binarization
static TBinarizationMethod BinMethod = TBinarizationMethodNICK;
static cv::Size BinaryWindowSize = {25, 25};

// patch filtering
static cv::Size FilteringPatchSize = {2, 2};
static float MinPatchContrastValue = 30;
static TPatchFilteringCriteria PatchFileringCriteria = TPatchFilteringCriteriaFull;

// acc image
static TAccImageSumMethod AccImageSumMethod = TAccImageSumMethodAvg;
