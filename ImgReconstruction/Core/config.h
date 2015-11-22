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
static const int ComparisonEpsL2 = 35;
static const int ComparisonEpsL1 = 10;
static const int ComparisonEpsPHash = 5;
static const int ComparisonEpsAvgHash = 10;

static const int MaxPatchSideSize = 8;
static const float BlurMetricRadiusRatio = 0.2f;

static const std::string DebugWindowName = "Debug";
static const std::string BinarizedWindowName = "Binarized";
static const std::string SaveImgPath = "../../out/";

static const cv::Point PatchOffset = cv::Point(1, 1);
static const cv::Size GaussianKernelSize = cv::Size(0, 0);

static const TBlurMeasureMethod BlurMeasureMethod = TBlurMeasureMethodStandartDeviation;
static const TImageCompareMetric CompMetric = TImageCompareMetricL1;

static const TBinarizationMethod BinMethod = TBinarizationMethodNiBlack;
static const cv::Size BinaryWindowSize = cv::Size(25, 25);
