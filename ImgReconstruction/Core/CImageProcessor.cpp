//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CTimeLogger.hpp"
#include "CDocumentBinarizer.hpp"

#ifdef DEBUG
    #define SHOW_BLUR_MAP 0
    #define SHOW_SIMILAR_PATCHES 0
    #define SHOW_RESIZED 1
#endif

void CImageProcessor::StartProcessingChain(const CImage& img)
{
    _mainImage = CImagePatch();
    _mainImage.SetGrayImage(img);
    
    BuildBinImage(img);
    BuildSdImage(img);
    
    ConfigureWindow(img);
}

void CImageProcessor::WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect)
{
#if SHOW_BLUR_MAP
    ProcessShowBlurMap(patchRect);
#elif SHOW_SIMILAR_PATCHES
    ProcessShowSimilarPatches(patchRect);
#elif SHOW_RESIZED
    ProcessShowResized(patchRect);
#endif
}

#pragma mark - Private

void CImageProcessor::ProcessShowBlurMap(const cv::Rect &patchRect)
{
    CImagePatch selectedPatch = FetchPatch(patchRect);
    std::vector<CImagePatch> patches = FetchPatches(patchRect);
    CTimeLogger::StartLogging("Show blur map:\n");
    
    std::vector<DrawableRect> rectsToDraw;
    for (int i = 0; i < patches.size(); i++) {
        AddBlurValueRect(rectsToDraw, patches[i]);
    }
    
    _window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessShowSimilarPatches(const cv::Rect &patchRect)
{
    CImagePatch selectedPatch = FetchPatch(patchRect);
    
    std::vector<CImagePatch> patches = FetchPatches(patchRect);
    
    CTimeLogger::StartLogging("Similar patches:\n");
    CImageComparator imgComparator(CompMetric);
    
    std::vector<DrawableRect> rectsToDraw;
    int good = 0, bad = 0;
    for (int i = 0; i < patches.size(); i++) {
        
        int distance = imgComparator.Compare(selectedPatch, patches[i]);
        int eps = CompEpsForCompMetric(CompMetric);
        if (distance < eps) {
            // чем больше размытия, тем темнее рамка вокруг патча
            cv::Scalar color = RGB(0, patches[i].BlurValue(BlurMeasureMethod), 0);
            rectsToDraw.push_back({patches[i].GetFrame(), color});
            good++;
            
            std::cout << "\t" << std::setw(4) << good << ". Frame: " << patches[i].GetFrame() << " Distance: " \
            << std::setw(3) << distance << std::endl;
        } else {
            bad++;
        }
    }
    
    std::cout << "\nGood patches: " << good << std::endl;
    CTimeLogger::Print("Similar patches search:");
    
    _window.DrawRects(rectsToDraw);
}

void CImageProcessor::ProcessShowResized(const cv::Rect &patchRect)
{
    CImagePatch selectedPatch = FetchPatch(patchRect);
    std::vector<CImagePatch> patches = FetchPatches(patchRect);
    
    std::vector<CImagePatch> similarPatches;
    CImageComparator imgComparator(CompMetric);
    int eps = CompEpsForCompMetric(CompMetric);
    for (int i = 0; i < patches.size(); i++) {
        if (utils::hamming(patches[i].AvgHash(), selectedPatch.AvgHash()) == 0) {
            int distance = imgComparator.Compare(selectedPatch, patches[i]);
            if (distance < eps) {
                similarPatches.push_back(patches[i]);
            }
        }
    }
    
    CImage result;
    for (int i = 0; i < similarPatches.size(); i++) {
        CImage temp;
        cv::hconcat(similarPatches[i].BinImage(), similarPatches[i].GrayImage(), temp);
        if (result.cols > 0 && result.rows > 0) {
            cv::vconcat(result, temp, result);
        } else {
            result = temp;
        }
    }
    
    _window.Update(result);
}

void CImageProcessor::BuildBinImage(const CImage &img)
{
    // строим бинаризованное изображение
    CTimeLogger::StartLogging();
    
    CDocumentBinarizer binarizer(BinaryWindowSize, BinMethod, 2.f);
    CImage blurredImage;
    //        cv::GaussianBlur(img, blurredImage, GaussianKernelSize, 0.2, 0.2);
    cv::bilateralFilter(img, blurredImage, 2, 1, 1);
    CImage binarizedImage;
    binarizedImage = binarizer.Binarize(blurredImage);
    
    _binarizedWindow = CWindow(BinarizedWindowName, binarizedImage);
    _binarizedWindow.Show();
    _binarizedWindow.Update(binarizedImage);
    
    CTimeLogger::Print("Binarization: ");
    
    _mainImage.SetBinImage(binarizedImage);
}

void CImageProcessor::BuildSdImage(const CImage &img)
{
    // строим sd изображение
    CTimeLogger::StartLogging();
    
    CImage sdImage;
    sdImage = utils::SDFilter(img, cv::Size(MaxPatchSideSize, MaxPatchSideSize));
    
    _debugWindow.Show();
    _debugWindow.Update(sdImage);
    
    CTimeLogger::Print("SD filter: ");
    
    _mainImage.SetSdImage(sdImage);
}

void CImageProcessor::ConfigureWindow(const CImage& img)
{
    // изображение для вывода
    img.copyTo(_displayImage);
    // делаем цветным
    cv::cvtColor(_displayImage, _displayImage, CV_GRAY2RGBA);
    
    _window.Show();
    _window.SetMaxBoxSideSize(MaxPatchSideSize);
    _window.SetOriginalImage(_displayImage);
    _window.Update(_displayImage);
    _window.StartObservingMouse();
    _window.ObserveKeyboard();
}

CImagePatch CImageProcessor::FetchPatch(const cv::Rect &patchRect)
{
    cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
    CDocumentBinarizer binarizer = CDocumentBinarizer(patchSize);
    
    CImagePatch selectedPatch = CImagePatch();
    selectedPatch.SetGrayImage(CImage(_mainImage.GrayImage(), patchRect));
    selectedPatch.SetBinImage(CImage(_mainImage.BinImage(), patchRect));
    selectedPatch.BlurValue(BlurMeasureMethod);
    selectedPatch.StandartDeviation();
    if (CompMetric == TImageCompareMetricPHash) {
        selectedPatch.PHash();
    } else if (CompMetric == TImageCompareMetricAvgHash) {
        selectedPatch.AvgHash();
    }
    
//    std::cout << "-------\nSelectedPatch:\n" << selectedPatch << "\n-------" <<std::endl;
    
    return selectedPatch;
}

std::vector<CImagePatch> CImageProcessor::FetchPatches(const cv::Rect& patchRect)
{
    CTimeLogger::StartLogging();
    
    std::vector<CImagePatch> patches;
    CImage grayImage = _mainImage.GrayImage();
    CImage binImage = _mainImage.BinImage();
    CImage sdImage = _mainImage.SdImage();
    
    cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
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

void CImageProcessor::AddBlurValueRect(std::vector<DrawableRect>& rects, CImagePatch& imagePatch)
{
    double colorComp = imagePatch.BlurValue(BlurMeasureMethod);
    cv::Scalar color = RGB(colorComp, colorComp, colorComp);
    rects.push_back({imagePatch.GetFrame(), color, CV_FILLED});
}


#pragma mark - Utils

int CImageProcessor::CompEpsForCompMetric(TImageCompareMetric metric)
{
    switch (metric) {
        case TImageCompareMetricL1:
            return ComparisonEpsL1;
        case TImageCompareMetricL2:
            return ComparisonEpsL2;
        case TImageCompareMetricPHash:
            return ComparisonEpsPHash;
        case TImageCompareMetricAvgHash:
            return ComparisonEpsAvgHash;
        default:
            break;
    }
    
    return 0;
}
