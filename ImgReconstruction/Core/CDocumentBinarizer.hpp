//
//  CNiBlackBinarizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 10.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBinarizer.hpp"

typedef enum : int {
    TBinarizationMethodNICK,
    TBinarizationMethodNiBlack,
} TBinarizationMethod;

class CDocumentBinarizer: public IBinarizer
{
public:
    CDocumentBinarizer() {_patchSize = cv::Size(30, 30);};
    CDocumentBinarizer(const cv::Size& patchSize, TBinarizationMethod binMethod = TBinarizationMethodNICK) : _patchSize(patchSize), _binMethod(binMethod) {};
    
    virtual CImage Binarize(const CImage& img) const;
public:
    cv::Size _patchSize;
    TBinarizationMethod _binMethod;
};