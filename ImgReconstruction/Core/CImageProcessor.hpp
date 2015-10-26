//
//  CImageProcessor.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CWindow.hpp"
#include "CImagePatch.hpp"
#include "CImageComparator.hpp"

extern const std::string DebugWindowName;
extern const std::string BinarizedWindowName;

class CImageProcessor : public CWindowDelegate
{
public:
    CImageProcessor(const CWindow& window) : _window(window), _debugWindow(DebugWindowName), _binarizedWindow(BinarizedWindowName)
    {
        _window.delegate = this;
    }
    
    // Project specific
    void StartProcessingChain(const CImage& img);
    
    // Algorithms
    static std::vector<CImage> FetchSimilarPatches(const CImage& img, const cv::Rect& patchRect);
    static CImage FFT(const CImage& image);
    static double MeasureBlurWithFFTImage(const CImage& image);
    static CImage SDFilter(const CImage& image, const cv::Size& filterSize);
    static double StandartDeviation(const CImage& image);
    static int64_t PHash(const CImage& image);
    static int64_t AvgHash(const CImage& image);
    
    // utils
    static CImage GetPatchImageFromImage(const CImage& img, const cv::Rect& patchRect);
    static void SaveImage(const std::string path, const CImage& image);
    int CompEpsForCompMetric(TImageCompareMetric metric);
    
    // CWindowDelegate
    virtual void WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect);
private:
    CImagePatch _mainImage;
    CImage _displayImage;
    
    CWindow _window;
    CWindow _debugWindow;
    CWindow _binarizedWindow;
};