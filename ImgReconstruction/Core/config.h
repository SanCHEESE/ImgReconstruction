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
static const double ComparisonEpsL1 = 1500;
static const double ComparisonEpsL2 = 35;

static const int MaxPatchSideSize = 8;
static const float BlurMetricRadiusRatio = 0.4f;

static const std::string DebugWindowName = "Debug";
static const std::string BinarizedWindowName = "Binarized";
static const std::string SaveImgPath = "../../out/";

static const cv::Point PatchOffset = {1, 1};

static const TBlurMeasureMethod BlurMeasureMethod = TBlurMeasureMethodFD;
static const TImageCompareMetric CompMetric = TImageCompareMetricL1;
static const TPatchClusteringMethod ClusteringMethod = TPatchClusteringMethodPHash;
static const TBinarizationMethod BinMethod = TBinarizationMethodNICK;
static const cv::Size BinaryWindowSize = {25, 25};

// patch filtering
static const cv::Size FilteringPatchSize = {2, 2};
static const float MinPatchContrastValue = 30;
static const TPatchFilteringCriteria PatchFileringCriteria = TPatchFilteringCriteriaFull;

// acc image
static const TAccImageSumMethod AccImageSumMethod = TAccImageSumMethodAvg;
