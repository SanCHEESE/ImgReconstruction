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

#define IMAGE_OUTPUT_ENABLED 0
#define VERBOSE 0

#define ENABLE_GUI 0

#define SHOW_BLUR_MAP 0
#define HIGHLIGHT_SIMILAR_PATCHES 0
#define SHOW_SORTED_SIMILAR 0
#define REPLACE_SIMILAR_PATCHES 0
#define TEST_BLUR_METRICS 0
#define PROCESS_IMAGE 1

extern const std::string DebugWindowName;
extern const std::string BinarizedWindowName;

class CImageProcessor : public CWindowDelegate
{
public:
	CImageProcessor(const CWindow& window) : _window(window), _debugWindow(DebugWindowName), _binarizedWindow(BinarizedWindowName)
	{
		_window.delegate = this;
	}
    
    CImageProcessor() : _window(""), _debugWindow(""), _binarizedWindow("") {};
	
	// Project specific
    void StartProcessingChain(const CImage& img, const std::string& outputImageName);
	// CWindowDelegate
	virtual void WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect);
	
	// utils
    double CompEpsForCompMetric(TImageCompareMetric metric)
    {
        switch (metric) {
            case TImageCompareMetricL1:
                return ComparisonEpsL1;
            case TImageCompareMetricL2:
                return ComparisonEpsL2;
            default:
                break;
        }
        
        return 0;
    }
	
private:
    // test methods
	void ProcessShowBlurMap();
	void ProcessHighlightSimilarPatches(const cv::Rect& patchRect);
	void ProcessShowSortedSimilar(const cv::Rect& patchRect);
	void ProcessReplaceSimilarPatches(const cv::Rect &patchRect);
    void ProcessTestBlurMetrics();
    
    // main method
    void ProcessFixImage();
	
    // ui
	void BuildAndShowBinImage(const CImage& img, bool show);
	void BuildAndShowSdImage(const CImage& img, bool show);
	void ConfigureWindow(const CImage& img);

	// utils
	std::vector<CImagePatch> FetchPatches(const cv::Rect& patchRect);
	CImagePatch FetchPatch(const cv::Rect& patchRect);
	void AddBlurValueRect(std::vector<DrawableRect>& rects, CImagePatch& imagePatch);
	std::vector<CImagePatch> FindSimilarPatches(CImagePatch& targetPatch, std::vector<CImagePatch>& patches);
	std::map<uint64, std::vector<CImagePatch>> Classify(std::vector<CImagePatch>& patches);
    std::map<int, std::vector<CImagePatch>> Clusterize(const std::vector<CImagePatch>& aClass);
    std::vector<CImagePatch> FilterPatches(std::vector<CImagePatch>& patches);
	
    // misc
    std::string _resultImageName;
    
    // gui
	CImagePatch _mainImage;
	CImage _displayImage;
	CWindow _window;
	CWindow _debugWindow;
	CWindow _binarizedWindow;
};