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
    CImageComparator(TImageCompareMetric compMetric = TImageCompareMetricL1) : _compMetric(compMetric) {};
    virtual double Compare(const CImagePatch& patch1, const CImagePatch& patch2) const;
private:
    double CompareL1(const CImagePatch& patch1, const CImagePatch& patch2) const;
    double CompareL2(const CImagePatch& patch1, const CImagePatch& patch2) const;

    TImageCompareMetric _compMetric;
};
