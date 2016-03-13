//
//  CPatchFilter.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CPatchFilter.h"
#import "CTimeLogger.h"
#import "CDocumentBinarizer.h"

std::vector<CImagePatch> CPatchFilter::FilterPatches(const std::vector<CImagePatch>& patches) const
{
    CTimeLogger::StartLogging();
    
    std::vector<CImagePatch> filteredPatches;
    
    for (const CImagePatch& patch: patches) {
        bool passedBin = false;
        
        CImage grey2x2 = patch.GrayImage().GetResizedImage(_filterPatchSize);
        CImage bin2x2 = _binarizer->Binarize(grey2x2);
        for (int column = 0; column < bin2x2.GetSize().width; column++) {
            for (int row = 0; row < bin2x2.GetSize().height; row++) {
                passedBin = bin2x2.at<uchar>(column, row) < 255;
            }
        }
        
        bool passedContrast = false;
        if (passedBin) {
            passedContrast = utils::StandartDeviation(grey2x2) >= _minContrastValue;
        }
        
        if (passedContrast && passedBin) {
            filteredPatches.push_back(patch);
        }
    }
    
    
    std::cout << "Before filter: " << patches.size() << std::endl;
    std::cout << "After filter: " << filteredPatches.size() << std::endl;
    CTimeLogger::Print("Patch filtering: ");
    
    return filteredPatches;

}