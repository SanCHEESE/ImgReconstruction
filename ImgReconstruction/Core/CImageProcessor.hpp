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
    // CWindowDelegate
    virtual void WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect);
    
    // utils
    inline int CompEpsForCompMetric(TImageCompareMetric metric);
    
private:
    void ProcessShowBlurMap(const cv::Rect& patchRect);
    void ProcessShowSimilarPatches(const cv::Rect& patchRect);
    void ProcessShowResized(const cv::Rect& patchRect);
    
    void BuildBinImage(const CImage& img);
    void BuildSdImage(const CImage& img);
    void ConfigureWindow(const CImage& img);
    
    std::vector<CImagePatch> FetchPatches(const cv::Rect& patchRect);
    CImagePatch FetchPatch(const cv::Rect& patchRect);
    void AddBlurValueRect(std::vector<DrawableRect>& rects, CImagePatch& imagePatch);
    
    CImagePatch _mainImage;
    CImage _displayImage;
    
    CWindow _window;
    CWindow _debugWindow;
    CWindow _binarizedWindow;
};