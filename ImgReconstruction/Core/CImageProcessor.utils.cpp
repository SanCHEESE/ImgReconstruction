//
//  CImageProcessor.utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.h"
#include "CTimeLogger.h"
#include "CDocumentBinarizer.h"

void CImageProcessor::BuildBinImage(const CImage &img)
{
    CTimeLogger::StartLogging();
    
    cv::Size binaryWindowSize;
    TBinarizationMethod binMethod;
    
    _config.GetParam(BinMethodConfigKey).GetValue(binMethod);
    _config.GetParam(BinaryWindowSizeConfigKey).GetValue(binaryWindowSize);
    
    CDocumentBinarizer binarizer(binaryWindowSize, binMethod, 2.f);
    CImage blurredImage;
    cv::bilateralFilter(img, blurredImage, 2, 1, 1);
    CImage binarizedImage;
    binarizedImage = binarizer.Binarize(img.GetExtentImage(binaryWindowSize));
    
    CTimeLogger::Print("Binarization: ");
    
    _mainImage.SetBinImage(binarizedImage);
}

CImagePatch CImageProcessor::FetchPatch(const cv::Rect &patchRect)
{
    TBlurMeasureMethod blurMeasureMethod;
    _config.GetParam(BlurMeasureMethodConfigKey).GetValue(blurMeasureMethod);
    
    cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
    CDocumentBinarizer binarizer = CDocumentBinarizer(patchSize);
    
    CImagePatch selectedPatch = CImagePatch();
    selectedPatch.SetGrayImage(CImage(_mainImage.GrayImage(), patchRect));
    selectedPatch.SetBinImage(CImage(_mainImage.BinImage(), patchRect));
    selectedPatch.BlurValue(blurMeasureMethod);
    selectedPatch.StandartDeviation();
    selectedPatch.PHash();
    selectedPatch.AvgHash();
    
    std::cout << "-------\nSelectedPatch:\n" << selectedPatch << "\n-------" <<std::endl;
    
    return selectedPatch;
}

std::vector<CImagePatch> CImageProcessor::FetchPatches(const cv::Rect& patchRect)
{
    CTimeLogger::StartLogging();
    
    std::vector<CImagePatch> patches;
    cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
    _mainImage.SetGrayImage(_mainImage.GrayImage().GetExtentImage(patchSize));
    _mainImage.SetBinImage(_mainImage.BinImage().GetExtentImage(patchSize));
    CImage grayImage = _mainImage.GrayImage();
    CImage binImage = _mainImage.BinImage();
    
    cv::Point patchOffset;
    _config.GetParam(PatchOffsetConfigKey).GetValue(patchOffset);
    
    CImage::CPatchIterator patchIterator = grayImage.GetPatchIterator(patchSize, patchOffset);
    CImage::CPatchIterator binPatchIterator = binImage.GetPatchIterator(patchSize, patchOffset);
    
    while (patchIterator.HasNext()) {
        CImagePatch imgPatch;
        imgPatch.SetBinImage(binPatchIterator.GetNext());
        imgPatch.SetGrayImage(patchIterator.GetNext());
        patches.push_back(imgPatch);
    }
    
    CTimeLogger::Print("Patch fetching: ");
    
    return patches;
}

std::map<int, std::vector<CImagePatch>> CImageProcessor::Clusterize(const std::vector<CImagePatch>& aClass)
{
    std::map<int, std::vector<CImagePatch>> clusters;
    auto aClassCopy = std::vector<CImagePatch>(aClass);
    
    TImageCompareMetric compMetric;
    TBrightnessEqualization brightnessEqualization;
    TCompSum compSum;
    double borderWeight;
    _config.GetParam(CompMetricConfigKey).GetValue(compMetric);
    _config.GetParam(BrightnessEqualizationConfigKey).GetValue(brightnessEqualization);
    _config.GetParam(CompSumConfigKey).GetValue(compSum);
    _config.GetParam(BorderSumWeightConfigKey).GetValue(borderWeight);
    
    CImageComparator compare = CImageComparator(compMetric, brightnessEqualization, compSum, borderWeight);
    int aClassIdx = 0;
    int eps = CompEpsForCompMetric(compMetric);
    for (int i = 0; i < aClassCopy.size(); i++) {
        
        std::vector<CImagePatch> similarPatches;
        similarPatches.push_back(aClassCopy[i]);
        aClassCopy[i].aClass = aClassIdx;
        
        for (int j = 1; j < aClassCopy.size(); j++) {
            double distance = compare(aClassCopy[i], aClassCopy[j]);
            if (distance < eps) {
                similarPatches.push_back(aClassCopy[j]);
                aClassCopy[j].aClass = aClassIdx;
                auto it = aClassCopy.begin() + j;
                aClassCopy.erase(it);
                j--;
            }
        }
        
        aClassCopy.erase(aClassCopy.begin());
        clusters[aClassIdx] = similarPatches;
        
        aClassIdx++;
        i--;
    }
    
    return clusters;
}

std::map<uint64, std::vector<CImagePatch>> CImageProcessor::Classify(std::vector<CImagePatch>& patches)
{
    CTimeLogger::StartLogging();
    
    std::map<uint64, std::vector<CImagePatch>> classes;
    TPatchClassifyingMethod classifyingMethod;
    _config.GetParam(ClassifyingMethodConfigKey).GetValue(classifyingMethod);
    
    auto classHash = [&](CImagePatch& patch) {
        if (classifyingMethod == TPatchClassifyingMethodPHash) {
            return patch.PHash();
        } else if (classifyingMethod == TPatchClassifyingMethodAvgHash) {
            return patch.AvgHash();
        }
        return UINT64_MAX;
    };
    
    for (CImagePatch& patch: patches) {
        auto aClass = classes.find(classHash(patch));
        if (aClass == classes.end()) {
            classes[classHash(patch)] = std::vector<CImagePatch>(1, patch);
        } else {
            (*aClass).second.push_back(patch);
        }
    }
    
    CTimeLogger::Print("Patch classification: ");
    
    return classes;
}