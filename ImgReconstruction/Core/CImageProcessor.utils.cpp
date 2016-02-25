//
//  CImageProcessor.utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CTimeLogger.hpp"
#include "CDocumentBinarizer.hpp"

CImagePatch CImageProcessor::FetchPatch(const cv::Rect &patchRect)
{
    cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
    CDocumentBinarizer binarizer = CDocumentBinarizer(patchSize);
    
    CImagePatch selectedPatch = CImagePatch();
    selectedPatch.SetGrayImage(CImage(_mainImage.GrayImage(), patchRect));
    selectedPatch.SetBinImage(CImage(_mainImage.BinImage(), patchRect));
    selectedPatch.BlurValue(BlurMeasureMethod);
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
    _mainImage.SetSdImage(_mainImage.SdImage().GetExtentImage(patchSize));
    CImage grayImage = _mainImage.GrayImage();
    CImage binImage = _mainImage.BinImage();
    CImage sdImage = _mainImage.SdImage();
    
    CImage::CPatchIterator patchIterator = grayImage.GetPatchIterator(patchSize, PatchOffset);
    CImage::CPatchIterator binPatchIterator = binImage.GetPatchIterator(patchSize, PatchOffset);
    CImage::CPatchIterator sdPatchIterator = sdImage.GetPatchIterator(patchSize, PatchOffset);
    
    while (patchIterator.HasNext()) {
        CImagePatch imgPatch;
        imgPatch.SetBinImage(binPatchIterator.GetNext());
        imgPatch.SetGrayImage(patchIterator.GetNext());
        imgPatch.SetSdImage(sdPatchIterator.GetNext());
        patches.push_back(imgPatch);
    }
    
    CTimeLogger::Print("Patch fetching: ");
    
    return patches;
}

std::vector<CImagePatch> CImageProcessor::FilterPatches(std::vector<CImagePatch>& patches)
{
    CTimeLogger::StartLogging();

    std::vector<CImagePatch> filteredPatches;
    
    cv::Size size2x2(2, 2);
    
    bool filterBin = (TPatchFilteringCriteriaBin & PatchFileringCriteria) == TPatchFilteringCriteriaBin;
    bool filterContrast = (TPatchFilteringCriteriaContrast & PatchFileringCriteria) == TPatchFilteringCriteriaContrast;
    
    CDocumentBinarizer b(size2x2, BinMethod, 1.);
    for (CImagePatch& patch: patches) {
        bool passedBin = !filterBin;

        CImage grey2x2 = patch.GrayImage().GetResizedImage(size2x2);
        if (filterBin) {
            CImage bin2x2 = b.Binarize(grey2x2);
            for (int column = 0; column < bin2x2.GetSize().width; column++) {
                for (int row = 0; row < bin2x2.GetSize().height; row++) {
                    passedBin = bin2x2.at<uchar>(column, row) < 255;
                }
            }
        }
        
        bool passedContrast = !filterContrast;
        if (filterContrast) {
            passedContrast = utils::StandartDeviation(grey2x2) >= MinPatchContrastValue;
        }
        
        if (passedContrast && passedBin) {
            filteredPatches.push_back(patch);
        }
    }
    
    
    std::cout << "Before filter: " << patches.size() << std::endl;
    std::cout << "After filter: " << filteredPatches.size() << std::endl;
    CTimeLogger::Print("Patch filtering: ");
    
    return filteredPatches;
}

std::vector<CImagePatch> CImageProcessor::FindSimilarPatches(CImagePatch& targetPatch, std::vector<CImagePatch>& patches)
{
    CTimeLogger::StartLogging();
    
    auto comparePatches = [](CImagePatch& patch1, CImagePatch& patch2) {
        if (ClassifyingMethod == TPatchClassifyingMethodPHash) {
            return utils::hamming<uint64>(patch1.PHash(), patch2.PHash());
        } else if (ClassifyingMethod == TPatchClassifyingMethodAvgHash) {
            return utils::hamming<uint64>(patch1.AvgHash(), patch2.AvgHash());
        }
        return INT_MAX;
    };
    
    std::vector<CImagePatch> similarPatches;
    CImageComparator compare(CompMetric);
    double eps = CompEpsForCompMetric(CompMetric);
    for (CImagePatch& patch: patches) {
        if (comparePatches(patch, targetPatch) == 0) {
            int distance = compare(targetPatch, patch);
            patch.distanceToTarget = distance;
            if (distance < eps) {
                patch.BlurValue(BlurMeasureMethod);
                similarPatches.push_back(patch);
            }
        }
    }
    
    std::cout << "Similar patches found: " << similarPatches.size() << std::endl;
    CTimeLogger::Print("Patch search: ");
    
    return similarPatches;
}

std::map<int, std::vector<CImagePatch>> CImageProcessor::Clusterize(const std::vector<CImagePatch>& aClass)
{
    std::map<int, std::vector<CImagePatch>> clusters;
    
    auto aClassCopy = std::vector<CImagePatch>(aClass);
    
    CImageComparator compare = CImageComparator(CompMetric);
    int aClassIdx = 0;
    for (int i = 0; i < aClassCopy.size(); i++) {
        
        std::vector<CImagePatch> similarPatches;
        similarPatches.push_back(aClassCopy[i]);
        aClassCopy[i].aClass = aClassIdx;
        
        for (int j = 1; j < aClassCopy.size(); j++) {
            double distance = compare(aClassCopy[i], aClassCopy[j]);
            if (distance < CompEpsForCompMetric(CompMetric)) {
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
    
    auto classHash = [](CImagePatch& patch) {
        if (ClassifyingMethod == TPatchClassifyingMethodPHash) {
            return patch.PHash();
        } else if (ClassifyingMethod == TPatchClassifyingMethodAvgHash) {
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