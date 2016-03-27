//
//  CPatchFetcher.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IPatchFetcher.h"
#include "IPatchFilter.h"
#include "CTimeLogger.h"

class CPatchFetcher: public IPatchFetcher
{
public:
    CPatchFetcher(const cv::Size& size, const cv::Point& offset, IPatchFilter* filter) : _size(size), _offset(offset), _filter(filter) {};
    
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
			imgPatch.SetGrayImage(patchIterator.GetNext());
            
			if (_filter->PatchPassesFilter(imgPatch)) {
				imgPatch.SetBinImage(binPatchIterator.GetNext());
				patches.push_back(imgPatch);
			} else {
				binPatchIterator.MoveNext();
			}
        }
        
        CTimeLogger::Print("Patch fetching: ");
        
        return patches;
    }
private:
	IPatchFilter* _filter;
    cv::Size _size;
    cv::Point _offset;
};