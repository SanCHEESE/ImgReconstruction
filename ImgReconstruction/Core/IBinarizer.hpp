//
//  IBinarizer.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//
#pragma once

class IBinarizer
{
public:
    virtual ~IBinarizer() {}
    virtual CImage Binarize(CImage& img) = 0;
};
