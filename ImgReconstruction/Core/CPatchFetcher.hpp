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

class CPatchFetcher : public IPatchFetcher
{
public:
	CPatchFetcher(const cv::Size& size, const cv::Point_<float>& offset, IPatchFilter* filter) : _size(size), _offset(offset), _filter(filter)
	{};

	virtual std::vector<CImage> FetchPatches(const CImage& img) const
	{
		CTimeLogger::StartLogging();

		IPatchIterator* patchIterator = img.GetIntPatchIterator(_size, _offset);
		std::vector<CImage> patches;
		while (patchIterator->HasNext()) {
			CImage patch = patchIterator->GetNext();
			if (patch.GetSize().width == 0 || patch.GetSize().height == 0) {
				continue;
			}
			if (_filter != 0) {
				if (_filter->PatchPassesFilter(patch)) {
					patches.push_back(patch);
				}
			} else {
				patches.push_back(patch);
			}
		}

		delete patchIterator;

		CTimeLogger::Print("Patch fetching: ");
		return patches;
	}


	virtual std::vector<CImagePatch> FetchPatches(const CImagePatch& imgPatch) const
	{
		CTimeLogger::StartLogging();

		CImage grayImage = imgPatch.GrayImage();
		CImage binImage = imgPatch.BinImage();

		IPatchIterator* patchIterator = grayImage.GetFloatPatchIterator(_size, _offset);
		IPatchIterator* binPatchIterator = binImage.GetFloatPatchIterator(_size, _offset);

		std::vector<CImagePatch> patches;
		while (patchIterator->HasNext()) {
			CImagePatch imgPatch;
			imgPatch.SetGrayImage(patchIterator->GetNext());

			if (_filter->PatchPassesFilter(imgPatch)) {
				imgPatch.SetBinImage(binPatchIterator->GetNext());
				patches.push_back(imgPatch);
				imgPatch.GrayImage().Save();
			} else {
				binPatchIterator->MoveNext();
			}
		}

		delete patchIterator;
		delete binPatchIterator;

		CTimeLogger::Print("Patch fetching: ");

		return patches;
	}
private:
	IPatchFilter* _filter;
	cv::Size _size;
	cv::Point_<float> _offset;
};