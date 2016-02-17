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
        bool passed = false;

        CImage grey2x2 = patch.GrayImage().GetResizedImage(size2x2);
        if (filterBin) {
            CImage bin2x2 = b.Binarize(grey2x2);
            for (int column = 0; column < bin2x2.GetSize().width; column++) {
                for (int row = 0; row < bin2x2.GetSize().height; row++) {
                    passed = bin2x2.at<uchar>(column, row) < 255;
                }
            }
        }
        
        if (filterContrast && passed) {
            passed = passed && (utils::StandartDeviation(grey2x2) >= MinPatchContrastValue);
        }
        
        if (passed) {
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
        if (ClusteringMethod == TPatchClusteringMethodPHash) {
            return utils::hamming<uint64>(patch1.PHash(), patch2.PHash());
        } else if (ClusteringMethod == TPatchClusteringMethodAvgHash) {
            return utils::hamming<uint64>(patch1.AvgHash(), patch2.AvgHash());
        }
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
            
//            std::cout << "Clustering distance " << i << " and " << j << ": " << distance << std::endl;;
        }
        
        aClassCopy.erase(aClassCopy.begin());
        clusters[aClassIdx] = similarPatches;
        
        aClassIdx++;
        i--;
    }
    
//    std::cout << "Patches left: " << aClass.size() - aClassCopy.size() << std::endl;
    
    return clusters;
}

//std::map<int, std::vector<CImagePatch>> CImageProcessor::Clusterize(std::vector<CImagePatch>& aClass)
//{
//    // симметричная матрица попарных расстояний
//    std::vector<std::vector<double>> distances(aClass.size());
//    for (int i = 0; i < aClass.size(); i++) {
//        distances[i] = std::vector<double>(aClass.size(), -1);
//    }
//    
//    // заполняем матрицу
//    CImageComparator cmp = CImageComparator(CompMetric);
//    for (int i = 0; i < aClass.size(); i++) {
//        for (int j = 0; j < aClass.size(); j++) {
//            if (i == j) {
//                distances[i][j] = 0;
//                continue;
//            }
//            distances[i][j] = cmp.Compare(aClass[i], aClass[j]);
//        }
//    }
//    
//    // извлекаем ближайшие патчи и расфасовываем по кластерам
//    std::map<int, std::vector<CImagePatch>> clusters;
//    for (int i = 0; i < aClass.size(); i++) {
//        std::vector<CImagePatch> similarPatches;
//        for (int j = 0; j < aClass.size(); j++) {
//            if (distances[i][j] > 0 && distances[i][j] < CompEpsForCompMetric(CompMetric)) {
//                similarPatches.push_back(aClass[j]);
//                aClass[j].aClass = i;
//                for (int k = 0; k < aClass.size(); k++) {
//                    distances[j][k] = -1;
//                    distances[k][j] = -1;
//                }
//            }
//        }
//        similarPatches.push_back(aClass[i]);
//        clusters[i] = similarPatches;
//    }
//    
//    // ищем патчи, которые не кластеризовали
//    for (int j = 0; j < aClass.size(); j++) {
//        // проходим по первой строке
//        if (distances[0][j] > 0) {
//            int minIdx = 0;
//            // ищем индекс минимального элемента в столбце
//            for (int i = 0; i < aClass.size(); i++) {
//                if (distances[i][j] < distances[minIdx][j] && distances[i][j] > 0) {
//                    minIdx = i;
//                }
//            }
//            auto cluster = clusters.find(minIdx);
//            if (cluster == clusters.end()) {
//                clusters[minIdx] = std::vector<CImagePatch>(1, aClass[j]);
//            } else {
//                cluster->second.push_back(aClass[j]);
//            }
//        }
//    }
//    
//    return clusters;
//}

std::map<uint64, std::vector<CImagePatch>> CImageProcessor::Classify(std::vector<CImagePatch>& patches)
{
    CTimeLogger::StartLogging();
    
    std::map<uint64, std::vector<CImagePatch>> classes;
    
    auto classHash = [](CImagePatch& patch) {
        if (ClusteringMethod == TPatchClusteringMethodPHash) {
            return patch.PHash();
        } else if (ClusteringMethod == TPatchClusteringMethodAvgHash) {
            return patch.AvgHash();
        }
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
    
#ifdef IMAGE_OUTPUT_ENABLED
    
    
    
#endif
    
    return classes;
}