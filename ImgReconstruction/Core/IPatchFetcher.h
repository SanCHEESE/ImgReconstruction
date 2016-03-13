//
//  IPatchFetcher.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

class IPatchFetcher: public IImageSubprocessor
{
public:
    virtual ~IPatchFetcher() {};
    virtual std::vector<CImagePatch> FetchPatches(const CImagePatch& imgPatch) const = 0;
};