//
//  CImagePatch.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 24.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageClassifier.hpp"
#include "CBlurMeasurer.hpp"

class CImagePatch
{
public:
    CImagePatch() {Initialize();}
    CImagePatch(const CImage& grayImage, const CImage& binImage, const CImage& sdImage) {Initialize();SetGrayImage(grayImage);SetBinImage(binImage);SetSdImage(sdImage);};
    
    // lazy accessors
    int ImgClass();
    double BlurValue(TBlurMeasureMethod method);
    double StandartDeviation();
    
    // accessors
    CImage GrayImage() const {return _grayImage;}
    CImage BinImage() const {return _binImage;}
    CImage SdImage() const {return _sdImage;}
    
    // setters
    void SetGrayImage(const CImage& image) {
        image.copyTo(_grayImage);
        _frame = _grayImage.GetFrame();
    };
    void SetBinImage(const CImage& image) {
        image.copyTo(_binImage);
        _frame = _binImage.GetFrame();
    };
    void SetSdImage(const CImage& image) {
        image.copyTo(_sdImage);
        _frame = _binImage.GetFrame();
    };

    int GetImgClass() const {return _imgClass;};
    double GetBlurValue() const {return _blurValue;};
    double GetStandartDeviation() const {return _standartDeviation;};
    cv::Rect GetFrame() const {return _frame;};
    
private:
    void Initialize();
    int CalculateImgClass() const;
    double CalculateBlurValue(TBlurMeasureMethod method) const;
    double CalculateStandartDeviation() const;
    
    cv::Rect _frame;
    CImage _grayImage;
    CImage _binImage;
    CImage _sdImage;
    int _imgClass;
    double _blurValue;
    double _standartDeviation;
};

struct Greater
{
    inline bool operator() (const CImagePatch& patch1, const CImagePatch& patch2)
    {
        return patch1.GetBlurValue() > patch2.GetBlurValue();
    }
};
