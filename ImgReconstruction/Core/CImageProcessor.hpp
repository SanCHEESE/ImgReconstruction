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
	
	// Project specific
	void StartProcessingChain(const CImage& img);
	// CWindowDelegate
	virtual void WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect);
	
	// utils
	static inline int CompEpsForCompMetric(TImageCompareMetric metric);
	
private:
    // test methods
	void ProcessShowBlurMap(const cv::Rect& patchRect);
	void ProcessHighlightSimilarPatches(const cv::Rect& patchRect);
	void ProcessShowSortedSimilar(const cv::Rect& patchRect);
	void ProcessReplaceSimilarPatches(const cv::Rect &patchRect);
    void ProcessTestBlurMetrics();
    
    // main method
    void ProcessFixImage();
	
	void BuildAndShowBinImage(const CImage& img, bool show);
	void BuildAndShowSdImage(const CImage& img, bool show);
	void ConfigureWindow(const CImage& img);
	
	std::deque<CImagePatch> FetchPatches(const cv::Rect& patchRect);
	CImagePatch FetchPatch(const cv::Rect& patchRect);
	void AddBlurValueRect(std::deque<DrawableRect>& rects, CImagePatch& imagePatch);
	std::deque<CImagePatch> FindSimilarPatches(CImagePatch& targetPatch, std::deque<CImagePatch>& patches);
	std::map<uint64, std::deque<CImagePatch> > FetchClusters(std::deque<CImagePatch>& patches);
	
	CImagePatch _mainImage;
	CImage _displayImage;
	
	CWindow _window;
	CWindow _debugWindow;
	CWindow _binarizedWindow;
};