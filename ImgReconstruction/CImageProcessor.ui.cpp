//
//  CImageProcessor.ui.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CTimeLogger.hpp"
#include "CDocumentBinarizer.hpp"

void CImageProcessor::BuildBinImage(const CImage &img)
{
    // строим бинаризованное изображение
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

void CImageProcessor::BuildSdImage(const CImage &img)
{
    // строим sd изображение
    CTimeLogger::StartLogging();
    
    int patchSideSize;
    _config.GetParam(MaxPatchSideSizeConfigKey).GetValue(patchSideSize);
    
    CImage sdImage = img.GetSDImage({patchSideSize, patchSideSize});
    
    CTimeLogger::Print("SD filter: ");
    
    _mainImage.SetSdImage(sdImage);
}

void CImageProcessor::ConfigureWindow(const CImage& img)
{
    // изображение для вывода
    img.copyTo(_displayImage);
    // делаем цветным
    cv::cvtColor(_displayImage, _displayImage, CV_GRAY2RGBA);
    
    int patchSideSize;
    _config.GetParam(MaxPatchSideSizeConfigKey).GetValue(patchSideSize);
    
    _window.SetMaxBoxSideSize(patchSideSize);
    _window.SetOriginalImage(_displayImage);
    _window.ShowAndUpdate(_displayImage);
    _window.StartObservingMouse();
    _window.ObserveKeyboard();
}
