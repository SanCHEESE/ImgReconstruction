//
//  CPatchFilter.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IPatchFilter.h"
#include "IBinarizer.h"
#include "CTimeLogger.h"

class CPatchFilter: public IPatchFilter
{
public:
    CPatchFilter(IBinarizer *binarizer, double minContrastValue, const cv::Size& filterPatchSize = {2, 2}) :
        _binarizer(binarizer),
        _minContrastValue(minContrastValue),
        _filterPatchSize(filterPatchSize) {};

	virtual bool PatchPassesFilter(const CImagePatch& patch) const
	{
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

		return passedContrast && passedBin;
	}
    
    virtual std::vector<CImagePatch> FilterPatches(const std::vector<CImagePatch>& patches) const
    {
        CTimeLogger::StartLogging();
        
        std::vector<CImagePatch> filteredPatches;
        
        for (const CImagePatch& patch: patches) {
            if (PatchPassesFilter(patch)) {
                filteredPatches.push_back(patch);
            }
        }
        
        
     /*   std::cout << "Before filter: " << patches.size() << std::endl;
        std::cout << "After filter: " << filteredPatches.size() << std::endl;*/
        CTimeLogger::Print("Patch filtering: ");
        
        return filteredPatches;
    }
    
private:
    IBinarizer* _binarizer;
    cv::Size _filterPatchSize;
    double _minContrastValue;
};