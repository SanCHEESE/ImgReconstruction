//
//  ImageComparator.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "IImageComparator.hpp"

class CImageComparator : public IImageComparator
{
public:
	CImageComparator(TImageCompareMetric compMetric = TImageCompareMetricL1, TBrightnessEqualization brightnessEqualization = TBrightnessEqualizationMean) :
        _compMetric(compMetric), _brightnessEqualization(brightnessEqualization) {};
    virtual double operator()(const CImagePatch& patch1, const CImagePatch& patch2) const;
private:
	double CompareL1(const CImagePatch& patch1, const CImagePatch& patch2) const;
	double CompareL2(const CImagePatch& patch1, const CImagePatch& patch2) const;
    
    void EqualizeBrightness(CImage &img1, CImage &img2) const;
    void EqualizeBrightnessMean(CImage &img1, CImage &img2) const;
    void EqualizeBrightnessDynRange(CImage &img1, CImage &img2) const;
	
	TImageCompareMetric _compMetric;
    TBrightnessEqualization _brightnessEqualization;
};
