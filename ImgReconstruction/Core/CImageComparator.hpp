//
//  ImageComparator.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "IImageComparator.hpp"

typedef enum : int {
    TImageCompareMetricL1,
    TImageCompareMetricL2,
    TImageCompareMetricDCT,
} TImageCompareMetric;

class CImageComparator : public IImageComparator
{
public:
    CImageComparator(TImageCompareMetric compMetric = TImageCompareMetricL1) : _compMetric(compMetric) {};
    virtual double Compare(const CImage& img1, const CImage& img2) const;
private:
    double CompareL1(const CImage& img1, const CImage& img2) const;
    double CompareL2(const CImage& img1, const CImage& img2) const;
    double CompareLDCT(const CImage& img1, const CImage& img2) const;
    
    TImageCompareMetric _compMetric;
};
