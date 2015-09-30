//
//  CImageProcessor.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CWindow.hpp"

extern const std::string FftWindowName;

class CImageProcessor : public CWindowDelegate
{
public:
    CImageProcessor(const CWindow& window) : _window(window), _fftWindow(FftWindowName)
    {
        _window.delegate = this;
    }
    
    // Project specific
    void StartProcessingChain(const CImage& img);
    
    // Algorithms
    static std::vector<CImage> FetchSimilarPatches(const CImage& img, const cv::Rect& patchRect);
    static CImage GetPatchImageFromImage(const CImage& img, const cv::Rect& patchRect);
    static CImage FFT(const CImage& image);
    static double MeasureBlurWithFFTImage(const CImage& image);
    
    // CWindowDelegate
    virtual void WindowDidSelectPatch(const CImage& img, const cv::Rect& patchRect);
private:
    CImage _image;
    CImage _binarizedImage;
    CImage _displayImage;
    
    CWindow _window;
    CWindow _fftWindow;
    float _progress;
};