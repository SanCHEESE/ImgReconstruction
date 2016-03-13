//
//  CParameter.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 25.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

class CParameter
{
public:
    CParameter() {
        _ResetValues();
    }
    
    CParameter(cv::Size value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(cv::Point value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(double value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(int value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(TImageCompareMetric value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(TBlurMeasureMethod value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(TPatchClassifyingMethod value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(TBinarizationMethod value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(TPatchFilteringCriteria value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(TAccImageSumMethod value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(TCompSum value) {
        _ResetValues();
        SetValue(value);
    }
    CParameter(TBrightnessEqualization value) {
        _ResetValues();
        SetValue(value);
    }
    
    
    void SetValue(cv::Size value) {
        assert(!_HasValue());
        _sizeValue = value;
        _hasSizeValue = true;
    }
    void SetValue(cv::Point value) {
        assert(!_HasValue());
        _pointValue = value;
        _hasPointValue = true;
    }
    void SetValue(double value) {
        assert(!_HasValue());
        _doubleValue = value;
        _hasDoubleValue = true;
    }
    void SetValue(int value) {
        assert(!_HasValue());
        _intValue = value;
        _hasIntValue = true;
    };
    
    void SetValue(TImageCompareMetric value) {
        SetValue((int)value);
    }
    void SetValue(TBlurMeasureMethod value) {
        SetValue((int)value);
    }
    void SetValue(TPatchClassifyingMethod value) {
        SetValue((int)value);
    }
    void SetValue(TBinarizationMethod value) {
        SetValue((int)value);
    }
    void SetValue(TPatchFilteringCriteria value) {
        SetValue((int)value);
    }
    void SetValue(TAccImageSumMethod value) {
        SetValue((int)value);
    }
    void SetValue(TBrightnessEqualization value) {
        SetValue((int)value);
    }
    void SetValue(TCompSum value) {
        SetValue((int)value);
    }
    
    void GetValue(cv::Size& value) const {value = _sizeValue;};
    void GetValue(int& value) const {value = _intValue;}
    void GetValue(double& value) const {value = _doubleValue;}
    void GetValue(cv::Point& value) const {value = _pointValue;};
    
    void GetValue(TImageCompareMetric& value) const {value = (TImageCompareMetric)_intValue;}
    void GetValue(TBlurMeasureMethod& value) const {value = (TBlurMeasureMethod)_intValue;}
    void GetValue(TPatchClassifyingMethod& value) const {value = (TPatchClassifyingMethod)_intValue;}
    void GetValue(TBinarizationMethod& value) const {value = (TBinarizationMethod)_intValue;}
    void GetValue(TPatchFilteringCriteria& value) const {value = (TPatchFilteringCriteria)_intValue;}
    void GetValue(TAccImageSumMethod& value) const {value = (TAccImageSumMethod)_intValue;}
    void GetValue(TBrightnessEqualization& value) const {value = (TBrightnessEqualization)_intValue;};
    void GetValue(TCompSum& value) const {value = (TCompSum)_intValue;}
    
    friend std::ostream& operator<<(std::ostream& os, const CParameter& param);
private:
    void _ResetValues() {
        _hasIntValue = false;
        _hasSizeValue = false;
        _hasPointValue = false;
        _hasDoubleValue = false;
    }
    bool _HasValue() const {
        return _hasIntValue || _hasPointValue || _hasSizeValue || _hasDoubleValue;
    }
    
    bool _hasIntValue;
    bool _hasSizeValue;
    bool _hasPointValue;
    bool _hasDoubleValue;
    
    cv::Size _sizeValue;
    cv::Point _pointValue;
    
    int _intValue;
    double _doubleValue;
};