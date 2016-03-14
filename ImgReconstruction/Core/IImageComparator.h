//
//  IImageComparator.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

class IImageComparator : public IImageSubprocessor
{
public:
    virtual ~IImageComparator() {};
    virtual bool Equal(const CImagePatch& patch1, const CImagePatch& patch2) const = 0;
};