#pragma once

#include <string>
#include <stdio.h>
#include <iostream>
#include <future>
#include <iomanip>
#include <vector>
#include <iterator>
#include <map>
#include <deque>
#include <ctime>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>

#define RGB(r, g, b) (cv::Scalar((b), (g), (r)))
#define RGBA(r, g, b, a) (cv::Scalar((b), (g), (r), (a)))

#define PRINT_BITS(num)  std::bitset<sizeof(uint64) * 8> b(num); \
	std::cout << b << std::endl;

typedef enum : int
{
	TInterpKernelBicubic,
	TInterpKernelLanczos,

	TInterpKernelNone
} TInterpKernelType;

typedef enum : int
{
	TImageCompareMetricL1,
	TImageCompareMetricL2,

	TImageCompareMetricNone
} TImageCompareMetric;

typedef enum : int
{
	TBlurMeasureMethodStandartDeviation,
	TBlurMeasureMethodDynamicRange,
	//TBlurMeasureMethodFFT,
	//TBlurMeasureMethodFD,

	TBlurMeasureMethodNone
} TBlurMeasureMethod;

typedef enum : int
{
	TBrightnessEqualizationMean,
	TBrightnessEqualizationDynRange,

	TBrightnessEqualizationNone
} TBrightnessEqualization;

typedef enum : int
{
	TCompSumStd,
	TCompSumBorder,

	TCompSumNone
} TCompSum;

typedef enum : int
{
	TBinarizationMethodNICK,
	TBinarizationMethodNiBlack,
	TBinarizationMethodAdaptiveGaussian,

	TBinarizationMethodNone
} TBinarizationMethod;

typedef enum : int
{
	TPatchClassifyingMethodAvgHash,
	TPatchClassifyingMethodPHash,

	TPatchClassifyingMethodNone
} TPatchClassifyingMethod;

typedef enum : int
{
	TAccImageSumMethodAvg,
	TAccImageSumMethodMedian,

	TAccImageSumMethodNone
} TAccImageSumMethod;