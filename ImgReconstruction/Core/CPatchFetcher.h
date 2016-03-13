//
//  CPatchFetcher.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IPatchFetcher.h"

class CPatchFetcher: public IPatchFetcher
{
    CPatchFetcher(const cv::Size& size, const cv::Point& offset) : _size(size), _offset(offset) {};
    
    virtual std::vector<CImagePatch> FetchPatches(const CImagePatch& imgPatch) const;
public:
    cv::Size _size;
    cv::Point _offset;
};