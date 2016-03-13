//
//  CPatchFetcher.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CPatchFetcher.h"
#include "CTimeLogger.h"

std::vector<CImagePatch> CPatchFetcher::FetchPatches(const CImagePatch& imgPatch) const
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