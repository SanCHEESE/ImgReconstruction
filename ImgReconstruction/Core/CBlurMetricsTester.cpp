#include "CBlurMetricsTester.h"

#include <CDynamicRangeBlurMeasurer.hpp>
#include <CFDBlurMeasurer.hpp>
#include <CFFTBlurMeasurer.hpp>
#include <CStdDeviationBlurMeasurer.hpp>

#include <CPatchFetcher.hpp>
#include <CImageExtender.hpp>

#include <CPatchFilter.hpp>
#include <CNiBlackBinarizer.hpp>

#include <utils.h>

#include <locale>

void CBlurMetricsTester::Test()
{
	std::cout.imbue(std::locale(""));

	const int bigPatchSize = 100;
	const int bigPatchOffset = 100;
	const int minContrastValue = 30;

	cv::Size patchSize = {bigPatchSize, bigPatchSize};
	cv::Point patchOffset = {bigPatchOffset, bigPatchOffset};
	IPatchFetcher *patchFetcher = new CPatchFetcher(patchSize, patchOffset, 0);

	auto patches = patchFetcher->FetchPatches(_image);
	std::random_shuffle(patches.begin(), patches.end());

	/* smooth images */
	int tag = 0;
	int kernelSize = 1;
	int maxKernelSize = 27;

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
		IPatchFilter *filter = new CPatchFilter(0, 30);

		std::vector<CImage> patchesToTest;
		for (CImage& blurredImage: blurredImages) {
			patchFetcher = new CPatchFetcher(patchSize, patchOffset, filter);
			auto patches = patchFetcher->FetchPatches(blurredImage);
			if (patches.size() == 0) {
				continue;
			}
			patchesToTest.push_back(patches[rand() % patches.size()]);

			delete patchFetcher;
		}
		for (TBlurMeasureMethod method = TBlurMeasureMethodStandartDeviation; method <= TBlurMeasureMethodFD; method = (TBlurMeasureMethod)((int)method + 1)) {
			IBlurMeasurer* blurMeasurer = BlurMeasurerForMethod(method);

			int errors = 0;
			int correct = 0;
			for (int i = 0; i < patchesToTest.size(); i++) {
				for (int j = 0; j < patchesToTest.size(); j++) {
					float blurValue1 = blurMeasurer->Measure(patchesToTest[i]);
					float blurValue2 = blurMeasurer->Measure(patchesToTest[j]);
					if (blurValue1 >= blurValue2 && i <= j || i >= j && blurValue1 <= blurValue2) {
						correct++;
					} else {
						errors++;
					}
				}
			}

			errors /= 2;
			correct /= 2;

			std::sort(patchesToTest.begin(), patchesToTest.end(), [&](const CImage& patch1, const CImage& patch2) -> bool {
				return blurMeasurer->Measure(patch1) > blurMeasurer->Measure(patch2);
			});

			utils::Stack(patchesToTest, 1).Save(MethodNameForMethod(method) + " {" + std::to_string(patchSideSize) + ", " + std::to_string(patchSideSize) + "}");

			std::cout << "Patch size: " << patchSize << "x" << patchSize << std::endl <<
				"Method " << MethodNameForMethod(method) << " "
				"Errors: " << (float)errors / (float)(errors + correct) * 100 << "%" << std::endl;
		}

		delete filter;

		//std::cout << std::endl;
	}
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
		default:
			break;
	}
	return 0;
}