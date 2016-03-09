//
//  CConfig.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 28.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CParameter.hpp"

class CConfig
{
public:
    CConfig() {
        _configMap[PatchOffsetConfigKey] = DefaultPatchOffset;
        _configMap[MaxPatchSideSizeConfigKey] = DefaultMaxPatchSideSize;
        _configMap[CompMetricConfigKey] = DefaultCompMetric;
        _configMap[ComparisonEpsL1ConfigKey] = DefaultComparisonEpsL1;
        _configMap[ComparisonEpsL2ConfigKey] = DefaultComparisonEpsL2;
        _configMap[BlurMetricRadiusRatioConfigKey] = DefaultBlurMetricRadiusRatio;
        _configMap[BlurMeasureMethodConfigKey] = DefaultBlurMeasureMethod;
        _configMap[ClassifyingMethodConfigKey] = DefaultClassifyingMethod;
        _configMap[BinMethodConfigKey] = DefaultBinMethod;
        _configMap[BinaryWindowSizeConfigKey] = DefaultBinaryWindowSize;
        _configMap[FilteringPatchSizeConfigKey] = DefaultFilteringPatchSize;
        _configMap[MinPatchContrastValueConfigKey] = DefaultMinPatchContrastValue;
        _configMap[PatchFileringCriteriaConfigKey] = DefaultPatchFileringCriteria;
        _configMap[AccImageSumMethodConfigKey] = DefaultAccImageSumMethod;
        _configMap[BrightnessEqualizationConfigKey] = DefaultBrightnessEqualization;
    };
    
    CConfig(const std::string& path) {
        ReadFromFile(path);
    };
    
    void SetParam(const std::string& key, CParameter param) {
        _configMap[key] = param;
    };
    CParameter GetParam(const std::string& key) const {
        return _configMap.at(key);
    };
    
    void WriteToFile(const std::string& path) const;
    void ReadFromFile(const std::string& path);
    
private:
    std::map<std::string, CParameter> _configMap;
};
