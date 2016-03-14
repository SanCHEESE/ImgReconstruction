//
//  CPatchFetcher.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IPatchFetcher.h"
#import "CTimeLogger.h"

class CPatchFetcher: public IPatchFetcher
{
    CPatchFetcher(const cv::Size& size, const cv::Point& offset) : _size(size), _offset(offset) {};
    
    virtual std::vector<CImagePatch> FetchPatches(const CImagePatch& imgPatch) const
    {
        CTimeLogger::StartLogging();
        
        CImage grayImage = imgPatch.GrayImage();
        CImage binImage = imgPatch.BinImage();
        
        CImage::CPatchIterator patchIterator = grayImage.GetPatchIterator(_size, _offset);
        CImage::CPatchIterator binPatchIterator = binImage.GetPatchIterator(_size, _offset);
        
        std::vector<CImagePatch> patches;
        while (patchIterator.HasNext()) {
            CImagePatch imgPatch;
            imgPatch.SetBinImage(binPatchIterator.GetNext());
            imgPatch.SetGrayImage(patchIterator.GetNext());
            patches.push_back(imgPatch);
        }
        
        CTimeLogger::Print("Patch fetching: ");
        
        return patches;
    }
public:
    cv::Size _size;
    cv::Point _offset;
};