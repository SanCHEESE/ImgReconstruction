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

class CPatchFilter : public IPatchFilter
{
public:
	CPatchFilter(IBinarizer *binarizer, float minContrastValue, const cv::Size& filterPatchSize = {2, 2}, float blackPixelsRatio = 0.25) :
		_binarizer(binarizer),
		_minContrastValue(minContrastValue),
		_filterPatchSize(filterPatchSize),
		_blackPixelsRatio(blackPixelsRatio)
	{};

	virtual bool PatchPassesFilter(const CImage& patch) const
	{
		bool passedBin = false;

		if (_binarizer) {
			CImage grey2x2 = patch.GetResizedImage(_filterPatchSize);

			CImage bin2x2 = _binarizer->Binarize(grey2x2);

			int blackPixels = 0;
			for (int row = 0; row < bin2x2.GetSize().height; row++) {
				for (int col = 0; col < bin2x2.GetSize().width; col++) {
					blackPixels += bin2x2.at<uchar>(row, col) < 255;
				}
			}

			/* black pixels takes more or eq than 25% */
			passedBin = ((float)blackPixels / (float)bin2x2.GetSize().area()) >= _blackPixelsRatio;
		} else {
			passedBin = true;
		}

		bool passedContrast = false;
		if (passedBin) {
			double min, max;
			cv::minMaxLoc(patch, &min, &max);
			passedContrast = (float)std::abs(max - min) >= _minContrastValue;
		}

		return passedContrast && passedBin;
	}

	virtual bool PatchPassesFilter(const CImagePatch& patch) const
	{
		return PatchPassesFilter(patch.GrayImage());
	}

	virtual std::vector<CImagePatch> FilterPatches(const std::vector<CImagePatch>& patches) const
	{
		CTimeLogger::StartLogging();

		std::vector<CImagePatch> filteredPatches;

		for (const CImagePatch& patch : patches) {
			if (PatchPassesFilter(patch)) {
				filteredPatches.push_back(patch);
			}
		}

		CTimeLogger::Print("Patch filtering: ");

		return filteredPatches;
	}

private:
	IBinarizer* _binarizer;
	cv::Size _filterPatchSize;
	float _minContrastValue;
	float _blackPixelsRatio;

};