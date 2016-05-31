//
//  CPatchFetcher.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include <IPatchFetcher.h>
#include <IPatchFilter.h>
#include <IInterpolationKernel.h>

class CPatchFetcher : public IPatchFetcher
{
public:
	CPatchFetcher(const cv::Size& size, const cv::Point_<float>& offset,
		const IPatchFilter* const filter, IInterpolationKernel* const kernel = 0) : _size(size), _offset(offset), _filter(filter), _kernel(kernel)
	{};

	virtual std::vector<CImage> FetchPatches(const CImage& img) const
	{

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
		return patches;
	}


	virtual std::vector<CImagePatch> FetchPatches(const CImagePatch& imgPatch) const
	{

		CImage grayImage = imgPatch.GrayImage();
		CImage binImage = imgPatch.BinImage();

		IPatchIterator* patchIterator = 0;
		IPatchIterator* binPatchIterator = 0;
		float temp;
		float fractX = modf(_offset.x, &temp);
		float fractY = modf(_offset.y, &temp);
		if (fractY > 0 || fractX > 0) {
			// use interpolation
			patchIterator = grayImage.GetFloatPatchIterator(_size, _offset, _kernel);
			binPatchIterator = binImage.GetFloatPatchIterator(_size, _offset, _kernel);
		} else {
			patchIterator = grayImage.GetIntPatchIterator(_size, _offset);
			binPatchIterator = binImage.GetIntPatchIterator(_size, _offset);
		}

		std::vector<CImagePatch> patches;
		while (patchIterator->HasNext()) {
			CImagePatch imgPatch;
			imgPatch.SetGrayImage(patchIterator->GetNext());

			if (_filter->PatchPassesFilter(imgPatch)) {
				CImage binImage = binPatchIterator->GetNext();
				cv::threshold(binImage, binImage, 125, 255, cv::THRESH_BINARY);
				imgPatch.SetBinImage(binImage);
				patches.push_back(imgPatch);
			} else {
				binPatchIterator->MoveNext();
			}
		}

		delete patchIterator;
		delete binPatchIterator;

		return patches;
	}
private:
	const IPatchFilter* const _filter;
	IInterpolationKernel* const _kernel;
	cv::Size _size;
	cv::Point_<float> _offset;
};