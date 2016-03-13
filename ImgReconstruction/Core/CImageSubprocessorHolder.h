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

#pragma once

static const std::string BlurMeasurerKey = "BlurMeasurerKey";
static const std::string ComparatorKey = "ComparatorKey";
static const std::string BinarizerKey = "BinarizerKey";
static const std::string PatchFilterKey = "PatchFilterKey";
static const std::string PatchClassifierKey = "PatchClassifierKey";
static const std::string PatchFetcherKey = "PatchFetcherKey";

class CImageSubprocessorHolder {
public:
    static CImageSubprocessorHolder& GetInstance() {static CImageSubprocessorHolder instance; return instance;};
    void Configure(const std::string& path);
    
    IImageSubprocessor* SubprocessorForKey(const std::string& key) {return _subprocessors[key];} ;
    
    IBlurMeasurer* Measurer() {return (IBlurMeasurer*)_subprocessors[BlurMeasurerKey];};
    IImageComparator* Comparator() {return (IImageComparator*)_subprocessors[ComparatorKey];};
    IBinarizer* Binarizer() {return (IBinarizer*)_subprocessors[BinarizerKey];};
    IPatchFilter* Filter() {return (IPatchFilter*)_subprocessors[PatchFilterKey];};
    IPatchClassifier* Classifier() {return (IPatchClassifier*)_subprocessors[PatchClassifierKey];}
    IPatchFetcher* PatchFetcher() {return (IPatchFetcher*)_subprocessors[PatchFetcherKey];}
    
private:
    CImageSubprocessorHolder();
    CImageSubprocessorHolder(CImageSubprocessorHolder const&) = delete;
    void operator=(CImageSubprocessorHolder const&) = delete;
    
    std::map<std::string, IImageSubprocessor*> _subprocessors;
};