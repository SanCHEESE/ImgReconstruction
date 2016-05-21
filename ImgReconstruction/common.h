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
	TImageCompareMetricFFT,
	TImageCompareMetricFD,

	TImageCompareMetricNone
} TImageCompareMetric;

typedef enum : int
{
	TBlurMeasureMethodStandartDeviation,
	TBlurMeasureMethodDynamicRange,
	TBlurMeasureMethodFFT,
	TBlurMeasureMethodFD,
	TBlurMeasureMethodDerivative,

	TBlurMeasureMethodNone
} TBlurMeasureMethod;

typedef enum : int
{
	TBlurMeasurerDerivativeCalcMethodMin,
	TBlurMeasurerDerivativeCalcMethodMax,
	TBlurMeasurerDerivativeCalcMethodAvg,
	TBlurMeasurerDerivativeCalcMethodSum,

	TBlurMeasurerDerivativeCalcMethodNone
} TBlurMeasurerDerivativeCalcMethod;

typedef enum : int
{
	TBinarizationMethodNICK,
	TBinarizationMethodNiBlack,

	TBinarizationMethodNone
} TBinarizationMethod;

typedef enum : int
{
	TPatchClassifyingMethodAvgHash,
	TPatchClassifyingMethodPHash,

	TPatchClassifyingMethodNone
} TPatchClassifyingMethod;
