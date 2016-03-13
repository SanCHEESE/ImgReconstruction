//
//  CPatchFilter.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IPatchFilter.h"
#import "IBinarizer.h"

class CPatchFilter: public IPatchFilter
{
public:
    CPatchFilter(IBinarizer *binarizer, double minContrastValue, const cv::Size& filterPatchSize = {2, 2}) :
        _binarizer(binarizer),
        _minContrastValue(minContrastValue),
        _filterPatchSize(filterPatchSize) {};
    virtual std::vector<CImagePatch> FilterPatches(const std::vector<CImagePatch>& patches) const;
private:
    IBinarizer* _binarizer;
    cv::Size _filterPatchSize;
    double _minContrastValue;
};