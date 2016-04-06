//
//  IPatchFetcher.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "common.h"
#include "IImageSubprocessor.h"
#include "CImagePatch.h"

class IPatchFetcher : public IImageSubprocessor
{
public:
	virtual ~IPatchFetcher() {};
	virtual std::vector<CImage> FetchPatches(const CImage& img) const = 0;
	virtual std::vector<CImagePatch> FetchPatches(const CImagePatch& imgPatch) const = 0;
};