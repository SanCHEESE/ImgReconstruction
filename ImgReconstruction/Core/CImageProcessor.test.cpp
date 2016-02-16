//
//  CImageProcessor-debug.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 23.01.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CTimeLogger.hpp"
#include "CDocumentBinarizer.hpp"

void CImageProcessor::ProcessTestBlurMetrics()
{
    int patchSideSize = 25;
    cv::Point origin(20, 25);
    TBlurMeasureMethod blurMeasureMethod = TBlurMeasureMethodFD;
    
    auto sortedPatches = std::vector<CImagePatch>();
    auto patches = std::vector<CImagePatch>();
    for (int i = 0; i < 12; i++) {
        CImagePatch patch;
        cv::Rect rect(origin.x, origin.y, patchSideSize, patchSideSize);
        patch.SetBinImage(_mainImage.BinImage()(rect));
        patch.SetSdImage(_mainImage.SdImage()(rect));
        patch.SetGrayImage(_mainImage.GrayImage()(rect));
        std::cout << "Blur value " << patch.BlurValue(blurMeasureMethod) << std::endl;
        patches.push_back(patch);
        sortedPatches.push_back(patch);
        origin.y += patchSideSize;
    }
    
    std::sort(sortedPatches.begin(), sortedPatches.end(), MoreBlur());
    
    std::cout << std::endl;
    
    CImage result;
    for (int i = 0; i < patches.size(); i++) {
        CImage temp;
        cv::Mat verticalSeparator(patches[i].GetFrame().height, 1, CV_8UC1, cv::Scalar(255));
        cv::hconcat(patches[i].GrayImage(), verticalSeparator, temp);
        cv::hconcat(temp, sortedPatches[i].GrayImage(), temp);
        
        std::cout << "Blur value " << sortedPatches[i].BlurValue(blurMeasureMethod) << std::endl;
        if (result.cols > 0 && result.rows > 0) {
            cv::Mat horisontalSeparator(1, patches[i].GetFrame().width * 2 + 1, CV_8UC1, cv::Scalar(255));
            cv::vconcat(result, horisontalSeparator, result);
            cv::vconcat(result, temp, result);
        } else {
            result = temp;
        }
    }
    
    result.Save("blurTest");
}

void CImageProcessor::ProcessShowBlurMap(const cv::Rect &patchRect)
{
    CImagePatch selectedPatch = FetchPatch(patchRect);
    std::vector<CImagePatch> patches = FetchPatches(patchRect);
    CTimeLogger::StartLogging("Show blur map:\n");
    
    std::vector<DrawableRect> rectsToDraw;
    for (auto& patch: patches) {
        AddBlurValueRect(rectsToDraw, patch);
    }
    
    _window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessHighlightSimilarPatches(const cv::Rect &patchRect)
{
    CImagePatch selectedPatch = FetchPatch(patchRect);
    
    std::vector<CImagePatch> patches = FetchPatches(patchRect);
    
    CTimeLogger::StartLogging("Highlight patches:\n");
    
    std::vector<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
    std::vector<DrawableRect> rectsToDraw;
    int good = 0;
    for (auto& similarPatch: similarPatches) {
        // чем больше размытия, тем темнее рамка вокруг патча
        cv::Scalar color(RGB(0, similarPatch.BlurValue(BlurMeasureMethod), 0));
        rectsToDraw.push_back({similarPatch.GetFrame(), color});
        good++;
        
        std::cout << "\t" << std::setw(4) << good << ". Frame: " << similarPatch.GetFrame() << std::endl;
    }
    
    std::cout << "\nGood patches: " << good << std::endl;
    CTimeLogger::Print("Patches to highlight search:");
    
    _window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessShowSortedSimilar(const cv::Rect &patchRect)
{
    CImagePatch selectedPatch = FetchPatch(patchRect);
    std::vector<CImagePatch> patches = FetchPatches(patchRect);
    patches = FilterPatches(patches);
    
    auto classes = Classify(patches);
    
    std::vector<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
    if (similarPatches.empty()) {
        return;
    }
    
    auto buildImage = [](const std::vector<CImagePatch>& similarPatches) {
        CImage result;
        for (auto& similarPatch: similarPatches) {
            CImage temp;
            cv::hconcat(similarPatch.BinImage(), similarPatch.GrayImage(), temp);
            if (result.cols > 0 && result.rows > 0) {
                cv::vconcat(result, temp, result);
            } else {
                result = temp;
            }
        }
        return result;
    };
    
    std::sort(similarPatches.begin(), similarPatches.end(), LessSimilarity());
    CImage similarityDecreaseImg = buildImage(similarPatches);
    
    std::sort(similarPatches.begin(), similarPatches.end(), MoreBlur());
    CImage blurIncreaseImg =  buildImage(similarPatches);
    
    similarityDecreaseImg.Save("similarityDecrease");
    blurIncreaseImg.Save("blurIncrease");
}

void CImageProcessor::ProcessReplaceSimilarPatches(const cv::Rect &patchRect)
{
    CImagePatch selectedPatch = FetchPatch(patchRect);
    std::vector<CImagePatch> patches = FetchPatches(patchRect);
    
    // извлекаем похожие патчи
    std::vector<CImagePatch> similarPatches = FindSimilarPatches(selectedPatch, patches);
    
    if (similarPatches.empty()) {
        return;
    }
    
    CTimeLogger::StartLogging();
    
    // сортируем по резкости
    std::sort(similarPatches.begin(), similarPatches.end(), MoreBlur());
    
    std::vector<int> labels;
    std::vector<float> data;
    int i = 0;
    for (const CImagePatch& patch: similarPatches) {
        labels.push_back(i);
        data.push_back(patch.distanceToTarget);
    }
    cv::Mat labelsMat(labels, true);
    cv::Mat dataMat(data, true);
    
    cv::TermCriteria::Type termCriteriaType = cv::TermCriteria::Type::EPS;
    cv::TermCriteria criteria(termCriteriaType,  10, 1);
    int k = 5;
    std::vector<float> centers;
    cv::kmeans(dataMat, k, labels, criteria, 10, cv::KMEANS_RANDOM_CENTERS, centers);
    
    if (labelsMat.isContinuous()) {
        labels.assign(labelsMat.datastart, labelsMat.dataend);
    }
    
    CImagePatch sharpPatch = similarPatches[0];
    std::vector<DrawableRect> rectsToDraw;
    CImage grayImage = _mainImage.GrayImage();
    
    // замещаем участки изображения
    for (CImagePatch& similarPatche: similarPatches) {
        CImage temp = grayImage(similarPatche.GetFrame());
        sharpPatch.GrayImage().copyTo(temp);
        cv::Scalar color = RGB(0, similarPatche.BlurValue(BlurMeasureMethod), 0);
        rectsToDraw.push_back({similarPatche.GetFrame(), color});
    }
    _mainImage.SetGrayImage(grayImage);
    
    CTimeLogger::Print("Image fix:");
    
    BuildAndShowBinImage(_mainImage.GrayImage(), true);
    _window.Update(_mainImage.GrayImage());
    
    _mainImage.GrayImage().Save("gray_fixed");
    _mainImage.BinImage().Save("bin_fixed");
}

void CImageProcessor::AddBlurValueRect(std::vector<DrawableRect>& rects, CImagePatch& imagePatch)
{
    double colorComp = imagePatch.BlurValue(BlurMeasureMethod);
    cv::Scalar color = RGB(colorComp, colorComp, colorComp);
    rects.push_back({imagePatch.GetFrame(), color, CV_FILLED});
}