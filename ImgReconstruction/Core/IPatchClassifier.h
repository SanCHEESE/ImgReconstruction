//
//  IClassifier.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

class IPatchClassifier: public IImageSubprocessor {
public:
    virtual IPatchClassifier() {};
};