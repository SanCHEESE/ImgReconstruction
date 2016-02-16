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

void CImageProcessor::BuildAndShowBinImage(const CImage &img, bool show)
{
    // строим бинаризованное изображение
    CTimeLogger::StartLogging();
    
    CDocumentBinarizer binarizer(BinaryWindowSize, BinMethod, 2.f);
    CImage blurredImage;
    cv::bilateralFilter(img, blurredImage, 2, 1, 1);
    CImage binarizedImage;
    binarizedImage = binarizer.Binarize(img.GetExtentImage(BinaryWindowSize));
    if (show) {
        _binarizedWindow.ShowAndUpdate(binarizedImage);
    }
    
    CTimeLogger::Print("Binarization: ");
    
    _mainImage.SetBinImage(binarizedImage);
}

void CImageProcessor::BuildAndShowSdImage(const CImage &img, bool show)
{
    // строим sd изображение
    CTimeLogger::StartLogging();
    
    CImage sdImage = img.GetSDImage({MaxPatchSideSize, MaxPatchSideSize});
    
    if (show) {
        _debugWindow.ShowAndUpdate(sdImage);
    }
    
    CTimeLogger::Print("SD filter: ");
    
    _mainImage.SetSdImage(sdImage);
}

void CImageProcessor::ConfigureWindow(const CImage& img)
{
    // изображение для вывода
    img.copyTo(_displayImage);
    // делаем цветным
    cv::cvtColor(_displayImage, _displayImage, CV_GRAY2RGBA);
    
    _window.SetMaxBoxSideSize(MaxPatchSideSize);
    _window.SetOriginalImage(_displayImage);
    _window.ShowAndUpdate(_displayImage);
    _window.StartObservingMouse();
    _window.ObserveKeyboard();
}
