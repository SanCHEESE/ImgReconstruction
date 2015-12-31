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
	static inline int CompEpsForCompMetric(TImageCompareMetric metric);
	
private:
	void ProcessShowBlurMap(const cv::Rect& patchRect);
	void ProcessHighlightSimilarPatches(const cv::Rect& patchRect);
	void ProcessShowSortedSimilar(const cv::Rect& patchRect);
	void ProcessReplaceSimilarPatches(const cv::Rect &patchRect);
	void ProcessFixImageStupid();
	void ProcessDrawHistogram(const cv::Rect &patchRect);
	
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