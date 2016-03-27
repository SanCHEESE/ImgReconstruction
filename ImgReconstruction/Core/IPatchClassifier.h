//
//  IClassifier.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "common.h"
#include "IImageSubprocessor.h"
#include "CImagePatch.h"

class IPatchClassifier: public IImageSubprocessor {
public:
    virtual ~IPatchClassifier() {};
    virtual std::map<uint64, std::vector<CImagePatch>> Classify(std::vector<CImagePatch>& patches) const = 0;
};