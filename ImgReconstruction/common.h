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
#include <unordered_set>
#include <fstream>
#include <ios>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>

#define ENABLE_CUDA 0

namespace cuda = cv::cuda;

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
	TImageCompareMetricL3,
	TImageCompareMetricFFT,
	TImageCompareMetricFD,

	TImageCompareMetricNone
} TImageCompareMetric;

typedef enum : int
{
	TBlurMeasureMethodDerivative,
	TBlurMeasureMethodDynamicRange,
	TBlurMeasureMethodFFT,
	TBlurMeasureMethodFD,
	TBlurMeasureMethodStandartDeviation,

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
