//
//  CImageProcessor.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CImageSubprocessorHolder.h"
#include "CImagePatch.h"

class CImageProcessor
{
public:
    CImageProcessor() : _iterCount(1) {
        _subprocHolder = &CImageSubprocessorHolder::GetInstance();
        _config = _subprocHolder->GetConfig();
    };
	
	// Project specific
    void ProcessImage(const CImage& img, const std::string& outImagePath);
    void SetIterCount(int iterCount) {_iterCount = iterCount;};
    
private:
    // main methods
    CImage RestoreImage();
    CImage RestoreImageIteratively(int iterCount, const CImage& img);
    void GenerateHelperImages(const CImage& img);
	void BuildBinImage(const CImage& img);

	// utils
    std::map<int, std::vector<CImagePatch>> Clusterize(const std::vector<CImagePatch>& aClass);
	
    // misc
    std::string _outImagePath;
    
	CImagePatch _mainImage;
    int _iterCount;
	cv::Size _origImageSize;
    
    CImageSubprocessorHolder* _subprocHolder;
    CConfig _config;
};