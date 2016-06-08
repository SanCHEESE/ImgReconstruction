#include "CBlurMetricsTester.h"

#include <IBinarizer.h>
#include <CNiBlackBinarizer.hpp>

#include <CDynamicRangeBlurMeasurer.hpp>
#include <CFDBlurMeasurer.hpp>
#include <CFFTBlurMeasurer.hpp>
#include <CStdDeviationBlurMeasurer.hpp>
#include <CDerivativeBlurMeasurer.hpp>

#include <CPatchFetcher.hpp>
#include <CImageExtender.hpp>

#include <CPatchFilter.hpp>
#include <CNiBlackBinarizer.hpp>

#include <utils.h>

#include <locale>
#include <ctime>

std::vector<CImage> RemoveBadPatches(const std::vector<CImage>& patches, int size)
{
	std::vector<CImage> result;
	for (const CImage& patch: patches) {
		if (patch.cols == size && patch.rows == size) {
			result.push_back(patch);
		}
	}

	return result;
}

void CBlurMetricsTester::Test()
{
	srand(time(0));

	std::cout.imbue(std::locale(""));

	const int bigPatchSize = 200;
	const int bigPatchOffset = 200;
	const int minContrastValue = 40;

	cv::Size patchSize = {bigPatchSize, bigPatchSize};
	cv::Point patchOffset = {bigPatchOffset, bigPatchOffset};
	IPatchFetcher *patchFetcher = new CPatchFetcher(patchSize, patchOffset, 0);

	auto patches = patchFetcher->FetchPatches(_image);
	patches = RemoveBadPatches(patches, bigPatchSize);
	std::random_shuffle(patches.begin(), patches.end());

	/* smooth images */
	int tag = 0;
	int kernelSize = 1;
	int maxKernelSize = 11;

	std::vector<CImage> blurredImages;
	for (int idx = kernelSize; idx <= maxKernelSize; idx += 2) {
		CImage tempImage;
		cv::GaussianBlur(patches[tag], tempImage, cv::Size(idx, idx), 0, 0);
		blurredImages.push_back(tempImage);
		//tempImage.Save();
		tag++;
	}

	delete patchFetcher;

	const int minPatchSize = 5;
	const int maxPatchSize = 26;
	/* iterate over patch sizes */
	for (int patchSideSize = minPatchSize; patchSideSize < maxPatchSize; patchSideSize++) {
		patchSize = {patchSideSize, patchSideSize};
		patchOffset = {patchSideSize, patchSideSize};
		IBinarizer *binarizer = new CNiBlackBinarizer(patchSize, -0.2f);
		IPatchFilter *filter = new CPatchFilter(binarizer, minContrastValue, {2, 2}, 0.75);

		std::vector<CImagePatch> patchesToTest;
		for (CImage& blurredImage: blurredImages) {
			patchFetcher = new CPatchFetcher(patchSize, patchOffset, filter);
			auto patches = patchFetcher->FetchPatches(blurredImage);
			patches = RemoveBadPatches(patches, patchSideSize);
			if (patches.size() == 0) {
				continue;
			}
			CImage randImg = patches[rand() % patches.size()];
			CImagePatch randPatch(randImg);
			randPatch.parentImage = &blurredImage;
			patchesToTest.push_back(randPatch);

			delete patchFetcher;
		}

		for (TBlurMeasureMethod method = TBlurMeasureMethodDerivative; method <= TBlurMeasureMethodStandartDeviation; method = (TBlurMeasureMethod)((int)method + 1)) {
			IBlurMeasurer* blurMeasurer = BlurMeasurerForMethod(method);

			int errors = 0;
			int correct = 0;
			for (int i = 0; i < patchesToTest.size(); i++) {
				for (int j = 0; j < patchesToTest.size(); j++) {
					float blurValue1 = patchesToTest[i].BlurValue(blurMeasurer);
					float blurValue2 = patchesToTest[j].BlurValue(blurMeasurer);
					if (blurValue1 >= blurValue2 && i <= j || i >= j && blurValue1 <= blurValue2) {
						correct++;
					} else {
						errors++;
					}
				}
			}

			std::sort(patchesToTest.begin(), patchesToTest.end(), [&](const CImagePatch& patch1, const CImagePatch& patch2) -> bool {
				return patch1.GetBlurValue() > patch2.GetBlurValue();
			});

			std::vector<CImage> patchesToDisplay(patchesToTest.size());
			for (auto& patch: patchesToTest) {
				patchesToDisplay.push_back(patch.GrayImage());
			}
			utils::Stack(patchesToDisplay, 1).Save(MethodNameForMethod(method) + " {" + std::to_string(patchSideSize) + ", " + std::to_string(patchSideSize) + "}");

			std::cout << "Patch size: " << patchSize << "x" << patchSize << std::endl <<
				"Method " << MethodNameForMethod(method) << " "
				"Errors: " << (float)errors / (float)(errors + correct) * 100 << "%" << std::endl;
		}

		delete filter;
		delete binarizer;
	}

	blurredImages.clear();
}

std::string CBlurMetricsTester::MethodNameForMethod(TBlurMeasureMethod method) const
{
	switch (method) {
		case TBlurMeasureMethodFFT:
			return "TBlurMeasureMethodFFT";
		case TBlurMeasureMethodDynamicRange:
			return "TBlurMeasureMethodDynamicRange";
		case TBlurMeasureMethodFD:
			return "TBlurMeasureMethodFD";
		case TBlurMeasureMethodStandartDeviation:
			return "TBlurMeasureMethodStandartDeviation";
		case TBlurMeasureMethodDerivative:
			return "TBlurMeasureMethodDerivative";
		default:
			break;
	}
	return 0;
}

IBlurMeasurer* CBlurMetricsTester::BlurMeasurerForMethod(TBlurMeasureMethod method) const
{
	switch (method) {
		case TBlurMeasureMethodFFT:
			return new CFFTBlurMeasurer(0.3);
		case TBlurMeasureMethodDynamicRange:
			return new CDynamicRangeBlurMeasurer();
		case TBlurMeasureMethodFD:
			return new CFDBlurMeasurer();
		case TBlurMeasureMethodStandartDeviation:
			return new CStdDeviationBlurMeasurer();
		case TBlurMeasureMethodDerivative:
			return new CDerivativeBlurMeasurer(2, 3, TBlurMeasurerDerivativeCalcMethodAvg);
		default:
			break;
	}
	return 0;
}