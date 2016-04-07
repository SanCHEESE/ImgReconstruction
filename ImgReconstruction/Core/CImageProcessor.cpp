//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.h"
#include "CTimeLogger.h"
#include "CAccImage.h"

void CImageProcessor::ProcessImage(const CImage& img, const std::string& outImagePath)
{
	_outImagePath = outImagePath;

	RestoreImageIteratively(_iterCount, img);
}

#ifdef __APPLE__
#pragma mark - Private
#endif

void CImageProcessor::GenerateHelperImages(const CImage& img)
{
	_origImageSize = img.GetSize();

	CImage extentImage = _subprocHolder->ImageExtender()->Extent(img);

	_mainImage = CImagePatch();
	_mainImage.SetGrayImage(extentImage);
	BuildBinImage(extentImage);
}

CImage CImageProcessor::RestoreImageIteratively(int iterCount, const CImage& img)
{
	CImage image = img;
	for (int iter = 0; iter < iterCount; iter++) {
		GenerateHelperImages(image);
		image = RestoreImage();
		image = image({0, 0, _origImageSize.width, _origImageSize.height});
	}

	image.Save(_outImagePath, 100, "");

	return image;
}

CImage CImageProcessor::RestoreImage()
{
	// get all image patches
	std::vector<CImagePatch> patches = _subprocHolder->PatchFetcher()->FetchPatches(_mainImage);

	//std::cout << "Total patches: " << patches.size() << std::endl;

	// calculating
	for (CImagePatch& patch : patches) {
		patch.BlurValue(_subprocHolder->BlurMeasurer());
	}

	// classification by PHash/AvgHash
	std::map<uint64, std::vector<CImagePatch>> classes = _subprocHolder->PatchClassifier()->Classify(patches);

	CAccImage accImage(_mainImage.GrayImage());

	for (auto &it : classes) {
		std::vector<CImagePatch> aClass = it.second;
		if (aClass.size() < 2) {
			// do not process classes with size of 1 object
			accImage.SetImageRegion(aClass[0].GrayImage());

			continue;
		} else {
			// ranking by sharpness inside a class
			auto clusters = Clusterize(aClass);

			for (auto& cluster : clusters) {
				auto clusterPatches = cluster.second;

				// sorting by blur increase
				std::sort(clusterPatches.begin(), clusterPatches.end(), MoreBlur());

				// copying to summing image
				CImagePatch bestPatch = clusterPatches[0];
				for (auto& patch : clusterPatches) {
					if (patch.GetFrame() != bestPatch.GetFrame()) {
						accImage.SetImageRegion(bestPatch.GrayImage(), patch.GetFrame());
					}
				}
			}
		}
	}

	return accImage.GetResultImage(_config.accImageSumMethod);
}
