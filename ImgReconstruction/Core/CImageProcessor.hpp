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
#include "CConfig.hpp"

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
        _iterCount = 1;
	}
    
    CImageProcessor() : _window(""), _debugWindow(""), _binarizedWindow(""), _iterCount(1) {_config = CConfig();};
	
	// Project specific
    void StartProcessingChain(const CImage& img, const std::string& outputImageName);
	// CWindowDelegate
	virtual void WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect);
	
	// utils
    int CompEpsForCompMetric(TImageCompareMetric metric)
    {
        int eps = 0;
        switch (metric) {
            case TImageCompareMetricL1:
                _config.GetParam(ComparisonEpsL1ConfigKey).GetValue(eps);
                break;
            case TImageCompareMetricL2:
                _config.GetParam(ComparisonEpsL2ConfigKey).GetValue(eps);
                break;
            default:
                break;
        }
//        std::cout << "eps = " << eps << std::endl;
        return eps;
    }
	
    CConfig& GetConfig() {return _config;}
    void SetIterCount(int iterCount) {_iterCount = iterCount;};
    
private:
    // test methods
	void ProcessShowBlurMap();
	void ProcessHighlightSimilarPatches(const cv::Rect& patchRect);
	void ProcessShowSortedSimilar(const cv::Rect& patchRect);
	void ProcessReplaceSimilarPatches(const cv::Rect &patchRect);
    void ProcessTestBlurMetrics();
    
    // main methods
    CImage ProcessRecoverImage();
    CImage ProcessRecoverImageIteratively(int iterCount, const CImage& img);
    void GenerateHelperImages(const CImage& img);
	
    // ui
	void BuildBinImage(const CImage& img);
	void BuildSdImage(const CImage& img);
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
    CConfig _config;
    
    // gui
	CImagePatch _mainImage;
	CImage _displayImage;
	CWindow _window;
	CWindow _debugWindow;
	CWindow _binarizedWindow;
    
    int _iterCount;
};