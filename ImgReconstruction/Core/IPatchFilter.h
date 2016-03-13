//
//  IFilter.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

class IPatchFilter: public IImageSubprocessor
{
public:
    virtual ~IPatchFilter() {};
    virtual std::vector<CImagePatch> FilterPatches(const std::vector<CImagePatch>& patches) const = 0;
};