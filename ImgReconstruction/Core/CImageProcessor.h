//
//  CImageProcessor.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CImagePatch.h"
#include "CImageComparator.h"
#include "CConfig.h"

#define IMAGE_OUTPUT_ENABLED 0
#define VERBOSE 0

class CImageProcessor
{
public:
    CImageProcessor() : _iterCount(1) {_config = CConfig();};
	
	// Project specific
    void ProcessImage(const CImage& img, const std::string& outputImageName);
	
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
        return eps;
    }
	
    CConfig& GetConfig() {return _config;}
    void SetIterCount(int iterCount) {_iterCount = iterCount;};
    
private:
    // main methods
    CImage RestoreImage();
    CImage RestoreImageIteratively(int iterCount, const CImage& img);
    void GenerateHelperImages(const CImage& img);
	
    // ui
	void BuildBinImage(const CImage& img);

	// utils
    CImagePatch FetchPatch(const cv::Rect& patchRect);
	std::vector<CImagePatch> FetchPatches(const cv::Rect& patchRect);
	std::map<uint64, std::vector<CImagePatch>> Classify(std::vector<CImagePatch>& patches);
    std::map<int, std::vector<CImagePatch>> Clusterize(const std::vector<CImagePatch>& aClass);
    std::vector<CImagePatch> FilterPatches(std::vector<CImagePatch>& patches);
	
    // misc
    std::string _resultImageName;
    CConfig _config;
    
	CImagePatch _mainImage;
    
    int _iterCount;
};