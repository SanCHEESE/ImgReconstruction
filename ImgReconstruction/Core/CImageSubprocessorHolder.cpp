//
//  CImageSubprocessorHolder.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include <fstream>

#include <json.hpp>

#include <CImageSubprocessorHolder.h>

#include <CPatchFilter.hpp>
#include <CPatchFetcher.hpp>

#include <CPHashPatchClassifier.hpp>
#include <CAvgHashPatchClassifier.hpp>

#include <CStdDeviationBlurMeasurer.hpp>
#include <CDynamicRangeBlurMeasurer.hpp>
#include <CDerivativeBlurMeasurer.hpp>

#include <CDynRangeBrightnessEqualizer.hpp>

#include <CL1ImageComparator.hpp>
#include <CL2ImageComparator.hpp>

#include <CNICKBinarizer.hpp>
#include <CNiBlackBinarizer.hpp>

#include <CLanczosKernel.hpp>
#include <CBicubicKernel.hpp>

#include <CImageExtender.hpp>

#include <config.h>

// for convenience
using json = nlohmann::json;

CBinarizer* Binarizer(TBinarizationMethod method, const cv::Size& patchSize, float k, float threshOffset)
{
	CBinarizer *binarizer = 0;
	switch (method) {
		case TBinarizationMethodNICK:
			binarizer = new CNICKBinarizer(patchSize, k, threshOffset);
			break;
		case TBinarizationMethodNiBlack:
			binarizer = new CNiBlackBinarizer(patchSize, k, threshOffset);
			break;
		default:
			assert(false);
			break;
	}
	return binarizer;
}

IImageComparator* Comparator(TImageCompareMetric metric, IBrightnessEqualizer* equalizer, int eps)
{
	IImageComparator* comparator = 0;
	switch (metric) {
		case TImageCompareMetricL1:
			comparator = new CL1ImageComparator(equalizer, eps);
			break;
		case TImageCompareMetricL2:
			comparator = new CL2ImageComparator(equalizer, eps);
			break;
		default:
			assert(false);
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
			assert(false);
			break;
	}
	return classifier;
}

IBlurMeasurer* Measurer(TBlurMeasureMethod method, int extentPixels = 0, float cutPercentage = 0, 
	TBlurMeasurerDerivativeCalcMethod calcMethod = TBlurMeasurerDerivativeCalcMethodMin)
{
	IBlurMeasurer* measurer = 0;
	switch (method) {
		case TBlurMeasureMethodDynamicRange:
			measurer = new CDynamicRangeBlurMeasurer();
			break;
		case TBlurMeasureMethodStandartDeviation:
			measurer = new CStdDeviationBlurMeasurer();
			break;
		case TBlurMeasureMethodDerivative:
			measurer = new CDerivativeBlurMeasurer(extentPixels, cutPercentage, calcMethod);
		default:
			assert(false);
			break;
	}
	return measurer;
}

IInterpolationKernel* InterpKernel(TInterpKernelType kernelType, int a, float b, float c)
{
	IInterpolationKernel* kernel = 0;
	switch (kernelType) {
		case TInterpKernelBicubic:
			kernel = new CBicubicKernel(a, b, c);
			break;
		case TInterpKernelLanczos:
			kernel = new CLanczosKernel(a);
			break;
		default:
			assert(false);
			break;
	}

	return kernel;
}

CImageSubprocessorHolder::CImageSubprocessorHolder()
{
	IBinarizer *filterBinarizer = Binarizer(DefaultBinMethod, DefaultFilteringPatchSize, DefaultFilteringBinK, DefaultThreshOffset);
	_subprocessors[FilterBinarizerKey] = (IImageSubprocessor *)filterBinarizer;
	IPatchFilter *patchFilter = new CPatchFilter(filterBinarizer, DefaultMinPatchContrastValue, DefaultFilteringPatchSize);
	_subprocessors[PatchFilterKey] = (IImageSubprocessor *)patchFilter;

	IPatchFetcher *patchFetcher = new CPatchFetcher(cv::Size(DefaultMaxPatchSideSize, DefaultMaxPatchSideSize), DefaultPatchOffset, patchFilter);
	_subprocessors[PatchFetcherKey] = (IImageSubprocessor *)patchFetcher;

	IBrightnessEqualizer *brightnessEqualizer = new CDynRangeBrightnessEqualizer();
	_subprocessors[CompBrightnessEqualizerKey] = (IImageSubprocessor *)brightnessEqualizer;
	IImageComparator* comparator = Comparator(DefaultCompMetric, brightnessEqualizer, DefaultComparisonEps);
	_subprocessors[ComparatorKey] = (IImageSubprocessor *)comparator;

	IBinarizer *binarizer = Binarizer(DefaultBinMethod, DefaultBinPatchSize, DefautBinK, DefaultThreshOffset);
	_subprocessors[BinarizerKey] = (IImageSubprocessor *)binarizer;

	IPatchClassifier* classifier = Classifier(DefaultClassifyingMethod);
	_subprocessors[PatchClassifierKey] = (IImageSubprocessor *)classifier;

	IBlurMeasurer* measurer = Measurer(DefaultBlurMeasureMethod);
	_subprocessors[BlurMeasurerKey] = (IImageSubprocessor *)measurer;

	IImageExtender* extender = new CImageExtender(DefaultBinPatchSize);
	_subprocessors[ImageExtenderKey] = (IImageSubprocessor *)extender;
};

void CImageSubprocessorHolder::Configure(const std::string &path)
{
	std::ifstream ifs(path);
	std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	auto json = json::parse(content);

	auto filterJson = json[FilterJsonKey];
	auto filterBinJson = filterJson[BinJsonKey];
	cv::Size patchSize = {filterBinJson[SizeJsonKey][WidthJsonKey], filterBinJson[SizeJsonKey][HeightJsonKey]};
	int binMethod = filterBinJson[MethodJsonKey];
	float threshOffset = DefaultThreshOffset;
	if (filterBinJson.find(OffsetJsonKey) != filterBinJson.end()) {
		threshOffset = filterBinJson[OffsetJsonKey];
	}

	IBinarizer *filterBinarizer = Binarizer((TBinarizationMethod)binMethod, patchSize, filterBinJson[KJsonKey], threshOffset);
	_subprocessors[FilterBinarizerKey] = (IImageSubprocessor *)filterBinarizer;
	IPatchFilter *patchFilter = new CPatchFilter(filterBinarizer, filterJson[ContrastJsonKey], patchSize);
	_subprocessors[PatchFilterKey] = (IImageSubprocessor *)patchFilter;

	auto patchFetchJson = json[FetchJsonKey];

	IInterpolationKernel *kernel = 0;
	if (patchFetchJson.find(KernelJsonKey) != patchFetchJson.end()) {
		// create kernel
		auto kernelJson = patchFetchJson[KernelJsonKey];
		int type = kernelJson[TypeJsonKey];
		int a = kernelJson[AJsonKey];
		float b = kernelJson[BJsonKey];
		float c = kernelJson[CJsonKey];
		kernel = InterpKernel((TInterpKernelType)type, a, b, c);
		_subprocessors[InterpolationKernelKey] = (IImageSubprocessor *)kernel;
	}
	IPatchFetcher *patchFetcher = new CPatchFetcher({patchFetchJson[SizeJsonKey][WidthJsonKey], patchFetchJson[SizeJsonKey][HeightJsonKey]},
	{patchFetchJson[OffsetJsonKey][XJsonKey], patchFetchJson[OffsetJsonKey][YJsonKey]},
		patchFilter, kernel);
	_subprocessors[PatchFetcherKey] = (IImageSubprocessor *)patchFetcher;

	auto comparatorJson = json[CompareJsonKey];
	auto summatorJson = comparatorJson[SumJsonKey];
	int compSum = summatorJson[MethodJsonKey];
	IBrightnessEqualizer *brightnessEqualizer = new CDynRangeBrightnessEqualizer();
	_subprocessors[CompBrightnessEqualizerKey] = (IImageSubprocessor *)brightnessEqualizer;
	int compMetric = comparatorJson[MetricJsonKey];
	IImageComparator* comparator = Comparator((TImageCompareMetric)compMetric, brightnessEqualizer, comparatorJson[EpsJsonKey]);
	_subprocessors[ComparatorKey] = (IImageSubprocessor *)comparator;

	auto binJson = json[BinJsonKey];
	binMethod = binJson[MethodJsonKey];

	IBinarizer *binarizer = Binarizer((TBinarizationMethod)binMethod,
	{binJson[SizeJsonKey][WidthJsonKey], binJson[SizeJsonKey][HeightJsonKey]}, binJson[KJsonKey], threshOffset);
	_subprocessors[BinarizerKey] = (IImageSubprocessor *)binarizer;

	auto blurJson = json[BlurJsonKey];
	int blurMeasureMethod = blurJson[MethodJsonKey];
	IBlurMeasurer* measurer = 0;
	if (blurMeasureMethod == TBlurMeasureMethodDerivative) {
		int calcMethod = blurJson[CalcJsonKey];
		measurer = Measurer((TBlurMeasureMethod)blurMeasureMethod, blurJson[ExtentJsonKey], blurJson[CutJsonKey], (TBlurMeasurerDerivativeCalcMethod)calcMethod);
	} else {
		measurer = Measurer((TBlurMeasureMethod)blurMeasureMethod);
	}
	_subprocessors[BlurMeasurerKey] = (IImageSubprocessor *)measurer;

	IImageExtender* extender = new CImageExtender({binJson[SizeJsonKey][WidthJsonKey], binJson[SizeJsonKey][HeightJsonKey]});
	_subprocessors[ImageExtenderKey] = (IImageSubprocessor *)extender;

	int classifyingMethod = json[ClassifierJsonKey];
	IPatchClassifier* classifier = Classifier((TPatchClassifyingMethod)classifyingMethod);
	_subprocessors[PatchClassifierKey] = (IImageSubprocessor *)classifier;

	_config.blurThresh = blurJson[ThreshJsonKey];
}

CImageSubprocessorHolder::~CImageSubprocessorHolder()
{
	this->Reset();
}

void CImageSubprocessorHolder::Reset()
{
	for (auto& subprocessor : _subprocessors) {
		delete subprocessor.second;
	}
}
