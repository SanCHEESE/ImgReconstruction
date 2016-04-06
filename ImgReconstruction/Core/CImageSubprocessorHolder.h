//
//  CImageSubprocessorHolder.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "IBlurMeasurer.h"
#include "IImageComparator.h"
#include "IBinarizer.h"
#include "IPatchFilter.h"
#include "IPatchClassifier.h"
#include "IPatchFetcher.h"
#include "IImageSummator.h"
#include "IBrightnessEqualizer.h"
#include "IImageExtender.h"

#pragma once

static const std::string BlurMeasurerKey = "BlurMeasurerKey";
static const std::string ComparatorKey = "ComparatorKey";
static const std::string CompImgSummatorKey = "CompareImgSummatorKey";
static const std::string CompBrightnessEqualizerKey = "CompBrightnessEqualizerKey";
static const std::string BinarizerKey = "BinarizerKey";
static const std::string PatchFilterKey = "PatchFilterKey";
static const std::string FilterBinarizerKey = "FilterBinarizerKey";
static const std::string PatchClassifierKey = "PatchClassifierKey";
static const std::string PatchFetcherKey = "PatchFetcherKey";
static const std::string ImageExtenderKey = "ImageExtenderKey";

// used to store not generalized classes
struct CConfig
{
	TAccImageSumMethod accImageSumMethod;
};

class CImageSubprocessorHolder
{
public:
	static CImageSubprocessorHolder& GetInstance() { static CImageSubprocessorHolder instance; return instance; };
	void Configure(const std::string& path);

	IImageSubprocessor* SubprocessorForKey(const std::string& key) { return _subprocessors[key]; };

	IBlurMeasurer* BlurMeasurer() { return (IBlurMeasurer*)_subprocessors[BlurMeasurerKey]; };
	IImageComparator* ImageComparator() { return (IImageComparator*)_subprocessors[ComparatorKey]; };
	IBinarizer* PatchBinarizer() { return (IBinarizer*)_subprocessors[BinarizerKey]; };
	IPatchFilter* PatchFilter() { return (IPatchFilter*)_subprocessors[PatchFilterKey]; };
	IPatchClassifier* PatchClassifier() { return (IPatchClassifier*)_subprocessors[PatchClassifierKey]; }
	IPatchFetcher* PatchFetcher() { return (IPatchFetcher*)_subprocessors[PatchFetcherKey]; }
	IImageExtender* ImageExtender() { return (IImageExtender*)_subprocessors[ImageExtenderKey]; }

	CConfig GetConfig() const { return _config; };
private:
	CImageSubprocessorHolder();
	~CImageSubprocessorHolder();
	CImageSubprocessorHolder(CImageSubprocessorHolder const&) = delete;
	void operator=(CImageSubprocessorHolder const&) = delete;

	void Reset();

	std::map<std::string, IImageSubprocessor*> _subprocessors;
	CConfig _config;
};