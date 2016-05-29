//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include <CImageProcessor.h>
#include <CTimeLogger.h>
#include <CAccImage.h>

#include <limits>

void CImageProcessor::ProcessImage(const CImage& img, const std::string& outImagePath)
{
	_outImagePath = outImagePath;

	RestoreImageIteratively(_iterCount, img);
}

void CImageProcessor::GenerateHelperImages(const CImage& img)
{
	_origImageSize = img.GetSize();

	CImage extentImage = _subprocHolder->ImageExtender()->Extent(img);
	CImage filteredImage;
	cv::bilateralFilter(extentImage, filteredImage, -1, 5, 5);
	filteredImage.Save();
	_mainImage = CImagePatch(filteredImage, _subprocHolder->PatchBinarizer()->Binarize(filteredImage));

	//_mainImage.BinImage().Save();
}

CImage CImageProcessor::RestoreImageIteratively(int iterCount, const CImage& img)
{
	CImage image = img;

	std::cout << _outImagePath << ": ";

	for (int iter = 0; iter < iterCount; iter++) {
		GenerateHelperImages(image);
		image = RestoreImage();
		image = image({0, 0, _origImageSize.width, _origImageSize.height});

		// image.Save("out-0" + std::to_string(iter));
	}

	image.Save(_outImagePath, 100, "");

	return image;
}

CImage CImageProcessor::RestoreImage()
{
	// get all image patches
	std::vector<CImagePatch> patches = _subprocHolder->PatchFetcher()->FetchPatches(_mainImage);

	std::cout << "Total patches: " << patches.size() << std::endl;

	// calculating
	float max = std::numeric_limits<float>::min();
	float min = std::numeric_limits<float>::max();

	for (CImagePatch& patch : patches) {
		patch.parentImage = const_cast<CImage*>(&_mainImage.GrayImage());
		float blurValue = patch.BlurValue(_subprocHolder->BlurMeasurer());
		max = MAX(max, blurValue);
		min = MIN(min, blurValue);
	}

	// normalize blur values
	for (int i = 0; i < patches.size(); i++) {
		patches[i].NormalizeBlurValue(min, max);
	}

	// classification by PHash/AvgHash
	std::map<uint64, std::vector<CImagePatch>> classes = _subprocHolder->PatchClassifier()->Classify(patches);

	CAccImage accImage(_mainImage.GrayImage(), _subprocHolder->InterpolationKernel(),
		_subprocHolder->CompBrightnessEqualizer(), _config.accOrigWeight, 1 - _config.accOrigWeight);

	int total = 0;

	for (auto &it : classes) {
		total += it.second.size();
		std::cout << _outImagePath << " processing " << total << " patches = " << (float)total / patches.size() * 100 << "%" << '\r' << std::flush;

		if (it.second.size() < 2) {
			// do not process classes with size of 1 object, 
			// instead we use original image pixel values

			continue;
		} else {
			// ranking by sharpness inside a class
			auto clusters = Clusterize(it.second);

			//CreateHistImage(clusters).Save("trash/" + std::to_string(it.first) + "-hist");

			for (auto& cluster : clusters) {
				if (cluster.second.size() <= 1) {
					continue;
				}

				// sorting by blur increase
				std::sort(cluster.second.begin(), cluster.second.end(), MoreBlur());

				//std::sort(clusterPatches.begin(), clusterPatches.end(), MoreBlur());

				int bestPatchIdx = 0;
				bool nonInterpolatedPatchFound = false;
				for (int i = 0; i < cluster.second.size(); i++) {
					if (!cluster.second[i].GrayImage().interpolated) {
						bestPatchIdx = i;
						nonInterpolatedPatchFound = true;
						break;
					}
				}

				if (!nonInterpolatedPatchFound) {
					continue;
				}

				// copying to summing image
				CImagePatch bestPatch = cluster.second[bestPatchIdx];
				for (auto& patch : cluster.second) {
					float blurThresh = std::abs((bestPatch.GetBlurValue() - patch.GetBlurValue()));
					// copy if threshhold in relatively big
					if (patch.GetFrame() != bestPatch.GetFrame() && _config.blurThresh < blurThresh) {
						accImage.UnitePatches(bestPatch, patch);
					}
				}
			}
		}
	}

	//CreateHistImage(classes).Save("trash/hash-hist");
	//accImage.CreateHistImage().Save("", 100, ".jpg");

	return accImage.GetResultImage();
}
