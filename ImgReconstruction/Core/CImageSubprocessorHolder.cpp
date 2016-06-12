//
//  CImageSubprocessorHolder.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include <fstream>

#include <CImageSubprocessorHolder.h>

#include <CPatchFilter.hpp>
#include <CPatchFetcher.hpp>

#include <CPHashPatchClassifier.hpp>
#include <CAvgHashPatchClassifier.hpp>

#include <CStdDeviationBlurMeasurer.hpp>
#include <CDynamicRangeBlurMeasurer.hpp>
#include <CDerivativeBlurMeasurer.hpp>
#include <CFDBlurMeasurer.hpp>

#include <CDynRangeBrightnessEqualizer.hpp>

#include <CL1ImageComparator.hpp>
#include <CL2ImageComparator.hpp>
#include <CL3ImageComparator.hpp>
#include <CBinImageComparator.hpp>

#include <CNICKBinarizer.hpp>
#include <CNiBlackBinarizer.hpp>

#include <CLanczosKernel.hpp>
#include <CBicubicKernel.hpp>

#include <CImageExtender.hpp>

#include <config.h>

#include <jsoncons/json.hpp>

CBinarizer* Binarizer(TBinarizationMethod method, const cv::Size& patchSize, float k)
{
	CBinarizer *binarizer = 0;
	switch (method) {
		case TBinarizationMethodNICK:
			binarizer = new CNICKBinarizer(patchSize, k);
			break;
		case TBinarizationMethodNiBlack:
			binarizer = new CNiBlackBinarizer(patchSize, k);
			break;
		default:
			assert(false, "Bin");
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
		case TImageCompareMetricL3:
			comparator = new CL3ImageComparator(equalizer, eps);
			break;
		case TImageCompareMetricBin:
			comparator = new CBinImageComparator(equalizer, eps);
			break;
		default:
			assert(false, "Metric");
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
			assert(false, "Classifier");
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
			break;
		case TBlurMeasureMethodFD:
			measurer = new CFDBlurMeasurer();
			break;
		default:
			assert(false, "Blur");
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
			assert(false, "Inter");
			break;
	}

	return kernel;
}

CImageSubprocessorHolder::CImageSubprocessorHolder()
{
	IBinarizer *filterBinarizer = Binarizer(DefaultBinMethod, DefaultFilteringPatchSize, DefaultFilteringBinK);
	_subprocessors[FilterBinarizerKey] = (IImageSubprocessor *)filterBinarizer;
	IPatchFilter *patchFilter = new CPatchFilter(filterBinarizer, DefaultMinPatchContrastValue, DefaultFilteringPatchSize);
	_subprocessors[PatchFilterKey] = (IImageSubprocessor *)patchFilter;

	IPatchFetcher *patchFetcher = new CPatchFetcher(cv::Size(DefaultMaxPatchSideSize, DefaultMaxPatchSideSize), DefaultPatchOffset, patchFilter);
	_subprocessors[PatchFetcherKey] = (IImageSubprocessor *)patchFetcher;

	IBrightnessEqualizer *brightnessEqualizer = new CDynRangeBrightnessEqualizer();
	_subprocessors[CompBrightnessEqualizerKey] = (IImageSubprocessor *)brightnessEqualizer;
	IImageComparator* comparator = Comparator(DefaultCompMetric, brightnessEqualizer, DefaultComparisonEps);
	_subprocessors[ComparatorKey] = (IImageSubprocessor *)comparator;

	IBinarizer *binarizer = Binarizer(DefaultBinMethod, DefaultBinPatchSize, DefautBinK);
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
	auto json = jsoncons::json::parse(content);

	// filter
	auto filterJson = json[FilterJsonKey];
	auto filterBinJson = filterJson[BinJsonKey];
	filterBinJson.get(SizeJsonKey);
	cv::Size patchSize = {filterBinJson[SizeJsonKey].as<int>(), filterBinJson[SizeJsonKey].as<int>()};
	float threshOffset = DefaultThreshOffset;
	if (filterBinJson.find(OffsetJsonKey) != filterBinJson.members().end()) {
		threshOffset = filterBinJson[OffsetJsonKey].as<float>();
	}
	IBinarizer *filterBinarizer = Binarizer((TBinarizationMethod)filterBinJson[MethodJsonKey].as<int>(), patchSize, filterBinJson[KJsonKey].as<float>());
	_subprocessors[FilterBinarizerKey] = (IImageSubprocessor *)filterBinarizer;
	IPatchFilter *patchFilter = new CPatchFilter(filterBinarizer, filterJson[ContrastJsonKey].as<float>(), patchSize);
	_subprocessors[PatchFilterKey] = (IImageSubprocessor *)patchFilter;

	// fetching
	auto patchFetchJson = json[FetchJsonKey];
	IInterpolationKernel *kernel = 0;
	if (patchFetchJson.find(KernelJsonKey) != patchFetchJson.members().end()) {
		// create kernel
		auto kernelJson = patchFetchJson[KernelJsonKey];
		int a = kernelJson[AJsonKey].as<int>();
		float b = kernelJson[BJsonKey].as<float>();
		float c = kernelJson[CJsonKey].as<float>();
		kernel = InterpKernel((TInterpKernelType)kernelJson[TypeJsonKey].as<int>(), a, b, c);
		_subprocessors[InterpolationKernelKey] = (IImageSubprocessor *)kernel;
	}
	IPatchFetcher *patchFetcher = new CPatchFetcher({patchFetchJson[SizeJsonKey].as<int>(), patchFetchJson[SizeJsonKey].as<int>()},
	{patchFetchJson[OffsetJsonKey].as<float>(), patchFetchJson[OffsetJsonKey].as<float>()}, patchFilter, kernel);
	_subprocessors[PatchFetcherKey] = (IImageSubprocessor *)patchFetcher;

	// compare
	auto comparatorJson = json[CompareJsonKey];
	IBrightnessEqualizer *brightnessEqualizer = new CDynRangeBrightnessEqualizer();
	_subprocessors[CompBrightnessEqualizerKey] = (IImageSubprocessor *)brightnessEqualizer;
	IImageComparator* comparator = Comparator((TImageCompareMetric)comparatorJson[MetricJsonKey].as<int>(), brightnessEqualizer, comparatorJson[EpsJsonKey].as<int>());
	_subprocessors[ComparatorKey] = (IImageSubprocessor *)comparator;

	// binarizer
	auto binJson = json[BinJsonKey];
	IBinarizer *binarizer = Binarizer((TBinarizationMethod)binJson[MethodJsonKey].as<int>(),
	{ binJson[SizeJsonKey].as<int>(), binJson[SizeJsonKey].as<int>() }, binJson[KJsonKey].as<float>());
	_subprocessors[BinarizerKey] = (IImageSubprocessor *)binarizer;

	// blue measure
	auto blurJson = json[BlurJsonKey];
	TBlurMeasureMethod blurMeasureMethod = (TBlurMeasureMethod)blurJson[MethodJsonKey].as<int>();
	IBlurMeasurer* measurer = 0;
	if (blurMeasureMethod == TBlurMeasureMethodDerivative) {
		TBlurMeasurerDerivativeCalcMethod calcMethod = (TBlurMeasurerDerivativeCalcMethod)blurJson[CalcJsonKey].as<int>();
		measurer = Measurer(blurMeasureMethod, blurJson[ExtentJsonKey].as<int>(), blurJson[CutJsonKey].as<float>(), calcMethod);
	} else {
		measurer = Measurer(blurMeasureMethod);
	}
	_subprocessors[BlurMeasurerKey] = (IImageSubprocessor *)measurer;

	// extender
	cv::Size size = {binJson[SizeJsonKey].as<int>(), binJson[SizeJsonKey].as<int>()};
	IImageExtender* extender = new CImageExtender(size);
	_subprocessors[ImageExtenderKey] = (IImageSubprocessor *)extender;

	// phash/avg hash classifier
	IPatchClassifier* classifier = Classifier((TPatchClassifyingMethod)json[ClassifierJsonKey].as<int>());
	_subprocessors[PatchClassifierKey] = (IImageSubprocessor *)classifier;

	// global config
	_config.blurThresh = blurJson[ThreshJsonKey].as<float>();
	_config.accOrigWeight = json[AccJsonKey].as<float>();
	_config.runCount = json[RunJsonKey].as<int>();
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
