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

#include <CMeanBrightnessEqualizer.hpp>
#include <CDynRangeBrightnessEqualizer.hpp>

#include <CStdImageSummator.hpp>
#include <CBorderImageSummator.hpp>

#include <CL1ImageComparator.hpp>
#include <CL2ImageComparator.hpp>
#include <CFDImageComparator.hpp>
#include <CFFTImageComparator.hpp>

#include <CAdaptiveGaussianBinarizer.hpp>
#include <CNICKBinarizer.hpp>
#include <CNiBlackBinarizer.hpp>

#include <CLanczosKernel.hpp>
#include <CBicubicKernel.hpp>

#include <CImageExtender.hpp>

#include <config.h>

// for convenience
using json = nlohmann::json;

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
		case TBinarizationMethodAdaptiveGaussian:
			binarizer = new CAdaptiveGaussianBinarizer(patchSize, k);
			break;
		default:
			break;
	}
	return binarizer;
}

IImageSummator* Summator(TCompSum method, float weight)
{
	IImageSummator *summator = 0;
	switch (method) {
		case TCompSumStd:
			summator = new CStdImageSummator();
			break;
		case TCompSumBorder:
			summator = new CBorderImageSummator(weight);
			break;
		default:
			break;
	}
	return summator;
}

IBrightnessEqualizer* BrightnessEqualizer(TBrightnessEqualization equalizationType)
{
	IBrightnessEqualizer* equalizer = 0;
	switch (equalizationType) {
		case TBrightnessEqualizationDynRange:
			equalizer = new CDynRangeBrightnessEqualizer();
			break;
		case TBrightnessEqualizationMean:
			equalizer = new CMeanBrightnessEqualizer();
			break;
		default:
			break;
	}

	return equalizer;
}

IImageComparator* Comparator(TImageCompareMetric metric, IBrightnessEqualizer* equalizer, IImageSummator* summator, int eps)
{
	IImageComparator* comparator = 0;
	switch (metric) {
		case TImageCompareMetricL1:
			comparator = new CL1ImageComparator(equalizer, summator, eps);
			break;
		case TImageCompareMetricL2:
			comparator = new CL2ImageComparator(equalizer, summator, eps);
			break;
		case TImageCompareMetricFD:
			comparator = new CFDImageComparator(0, 0, eps);
			break;
		case TImageCompareMetricFFT:
			comparator = new CFFTImageComparator(0, 0, eps);
		default:
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
			break;
	}
	return classifier;
}

IBlurMeasurer* Measurer(TBlurMeasureMethod method, float param)
{
	IBlurMeasurer* measurer = 0;
	switch (method) {
		//case TBlurMeasureMethodFD:
		//	measurer = new CFDBlurMeasurer();
		//	break;
		//case TBlurMeasureMethodFFT:
		//	measurer = new CFFTBlurMeasurer(param);
		//	break;
		case TBlurMeasureMethodDynamicRange:
			measurer = new CDynamicRangeBlurMeasurer();
			break;
		case TBlurMeasureMethodStandartDeviation:
			measurer = new CStdDeviationBlurMeasurer();
			break;
		default:
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

	IImageSummator *summator = Summator(DefaultCompSum, DefaultBorderSumWeight);
	_subprocessors[CompImgSummatorKey] = (IImageSubprocessor *)summator;
	IBrightnessEqualizer *brightnessEqualizer = BrightnessEqualizer(DefaultBrightnessEqualization);
	_subprocessors[CompBrightnessEqualizerKey] = (IImageSubprocessor *)brightnessEqualizer;
	IImageComparator* comparator = Comparator(DefaultCompMetric, brightnessEqualizer, summator, DefaultComparisonEps);
	_subprocessors[ComparatorKey] = (IImageSubprocessor *)comparator;

	IBinarizer *binarizer = Binarizer(DefaultBinMethod, DefaultBinPatchSize, DefautBinK);
	_subprocessors[BinarizerKey] = (IImageSubprocessor *)binarizer;

	IPatchClassifier* classifier = Classifier(DefaultClassifyingMethod);
	_subprocessors[PatchClassifierKey] = (IImageSubprocessor *)classifier;

	IBlurMeasurer* measurer = Measurer(DefaultBlurMeasureMethod, DefaultBlurMetricRadiusRatio);
	_subprocessors[BlurMeasurerKey] = (IImageSubprocessor *)measurer;

	IImageExtender* extender = new CImageExtender(DefaultBinPatchSize);
	_subprocessors[ImageExtenderKey] = (IImageSubprocessor *)extender;

	_config.accImageSumMethod = DefaultAccImageSumMethod;
};

void CImageSubprocessorHolder::Configure(const std::string &path)
{
	std::ifstream ifs(path);
	std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	auto json = json::parse(content);

	auto filterJson = json[PatchFilterJsonKey];
	auto filterBinJson = filterJson[BinJsonKey];
	cv::Size patchSize = {filterBinJson[PatchSizeJsonKey][WidthJsonKey], filterBinJson[PatchSizeJsonKey][HeightJsonKey]};
	int binMethod = filterBinJson[MethodJsonKey];
	IBinarizer *filterBinarizer = Binarizer((TBinarizationMethod)binMethod, patchSize, filterBinJson[KJsonKey]);
	_subprocessors[FilterBinarizerKey] = (IImageSubprocessor *)filterBinarizer;
	IPatchFilter *patchFilter = new CPatchFilter(filterBinarizer, filterJson[ContrastValueJsonKey], patchSize);
	_subprocessors[PatchFilterKey] = (IImageSubprocessor *)patchFilter;

	auto patchFetchJson = json[PatchFetchJsonKey];

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
	IPatchFetcher *patchFetcher = new CPatchFetcher({patchFetchJson[PatchSizeJsonKey][WidthJsonKey], patchFetchJson[PatchSizeJsonKey][HeightJsonKey]},
	{patchFetchJson[PatchOffsetJsonKey][XJsonKey], patchFetchJson[PatchOffsetJsonKey][YJsonKey]},
		patchFilter, kernel);
	_subprocessors[PatchFetcherKey] = (IImageSubprocessor *)patchFetcher;

	auto comparatorJson = json[ImageCompareJsonKey];
	auto summatorJson = comparatorJson[ImageSumJsonKey];
	int compSum = summatorJson[MethodJsonKey];
	IImageSummator *summator = Summator((TCompSum)compSum, summatorJson[SummatorWeightJsonKey]);
	_subprocessors[CompImgSummatorKey] = (IImageSubprocessor *)summator;
	int brightnessEqualization = comparatorJson[BrightnessEqualizeJsonKey];
	IBrightnessEqualizer *brightnessEqualizer = BrightnessEqualizer((TBrightnessEqualization)brightnessEqualization);
	_subprocessors[CompBrightnessEqualizerKey] = (IImageSubprocessor *)brightnessEqualizer;
	int compMetric = comparatorJson[MetricJsonKey];
	IImageComparator* comparator = Comparator((TImageCompareMetric)compMetric, brightnessEqualizer, summator, comparatorJson[EpsJsonKey]);
	_subprocessors[ComparatorKey] = (IImageSubprocessor *)comparator;

	auto binJson = json[BinJsonKey];
	binMethod = binJson[MethodJsonKey];
	IBinarizer *binarizer = Binarizer((TBinarizationMethod)binMethod,
	{binJson[PatchSizeJsonKey][WidthJsonKey], binJson[PatchSizeJsonKey][HeightJsonKey]},
		binJson[KJsonKey]);
	_subprocessors[BinarizerKey] = (IImageSubprocessor *)binarizer;

	auto blurJson = json[BlurMeasureJsonKey];
	int blurMeasureMethod = blurJson[MethodJsonKey];
	IBlurMeasurer* measurer = Measurer((TBlurMeasureMethod)blurMeasureMethod, blurJson[BlurRatioJsonKey]);
	_subprocessors[BlurMeasurerKey] = (IImageSubprocessor *)measurer;

	IImageExtender* extender = new CImageExtender({json[ExtenderPatchSizeJsonKey][WidthJsonKey], json[ExtenderPatchSizeJsonKey][HeightJsonKey]});
	_subprocessors[ImageExtenderKey] = (IImageSubprocessor *)extender;

	int classifyingMethod = json[ClassifierJsonKey];
	IPatchClassifier* classifier = Classifier((TPatchClassifyingMethod)classifyingMethod);
	_subprocessors[PatchClassifierKey] = (IImageSubprocessor *)classifier;

	int accSumMethod = json[AccSumJsonKey];
	_config.accImageSumMethod = (TAccImageSumMethod)accSumMethod;
	_config.blurThresh = blurJson[BlurThreshJsonKey];
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
