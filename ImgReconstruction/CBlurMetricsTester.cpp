#include "CBlurMetricsTester.h"

#include <stdlib.h>    
#include <time.h>      

#include <CDynamicRangeBlurMeasurer.hpp>
#include <CFDBlurMeasurer.hpp>
#include <CFFTBlurMeasurer.hpp>
#include <CStdDeviationBlurMeasurer.hpp>

#include <CPatchFetcher.hpp>
#include <CImageExtender.hpp>

#include <CPatchFilter.hpp>
#include <CNiBlackBinarizer.hpp>

#include <utils.h>

void CBlurMetricsTester::Test()
{
	const int bigPatchSize = 300;
	const int bigPatchOffset = 300;
	const int minContrastValue = 30;
	const int minPatchSize = 8;
	const int maxPatchSize = 26;

	srand(time(NULL));

	/* fetch big patches */
	CImageExtender extender({bigPatchSize, bigPatchSize});
	CImage extentImage = extender.Extent(_image);
	CPatchFetcher patchFetcher({bigPatchSize, bigPatchSize}, {bigPatchOffset , bigPatchOffset}, 0);
	auto bigPatches = patchFetcher.FetchPatches(extentImage);

	/* smooth images */
	int tag = 0;
	int kernelSize = 1;
	int maxKernelSize = 27;
	std::vector<CImage> temp;

	for (int kernelSize = 3, idx = 0; kernelSize <= maxKernelSize; kernelSize += 2, idx++) {
		CImage img = bigPatches[idx];
		cv::GaussianBlur(img, img, cv::Size(kernelSize, kernelSize), 0, 0);
		img.tag = tag;
		temp.push_back(img);
		tag++;
	}
	bigPatches = temp;

	/* iterate over patch sizes */
	for (int patchSize = minPatchSize; patchSize < maxPatchSize; patchSize++) {
		std::vector<CImage> patchesToTest;

		IBinarizer* binarizer = new CNiBlackBinarizer({patchSize, patchSize}, -0.2);
		IPatchFilter* filter = new CPatchFilter(binarizer, minContrastValue, {patchSize / 4, patchSize / 4});
		IPatchFetcher* patchFetcher = new CPatchFetcher({patchSize, patchSize}, {patchSize, patchSize}, filter);

		for (CImage& bigImage : bigPatches) {
			CImageExtender extender({patchSize, patchSize});
			CImage extended = extender.Extent(bigImage);

			auto patches = patchFetcher->FetchPatches(extended);

			if (patches.size() > 0) {
				int randPatch = rand() % patches.size();
				patchesToTest.push_back(patches[randPatch]);
				patchesToTest[patchesToTest.size() - 1].tag = bigImage.tag;
			}
		}

		//utils::Stack(patchesToTest, 1).Save();

		for (TBlurMeasureMethod method = TBlurMeasureMethodStandartDeviation; method <= TBlurMeasureMethodFD; method = (TBlurMeasureMethod)((int)method + 1)) {
			IBlurMeasurer* blurMeasurer = BlurMeasurerForMethod(method);

			for (int i = 0; i < patchesToTest.size(); i++) {
				std::cout << patchesToTest[i].tag << " " << blurMeasurer->Measure(patchesToTest[i]) << std::endl;
			}

			int errors = 0;
			int correct = 0;
			for (int i = 0; i < patchesToTest.size(); i++) {
				for (int j = 0; j < patchesToTest.size(); j++) {
					double blurValue1 = blurMeasurer->Measure(patchesToTest[i]);
					double blurValue2 = blurMeasurer->Measure(patchesToTest[j]);
					if (blurValue1 >= blurValue2 && patchesToTest[i].tag <= patchesToTest[j].tag) {
						correct++;
					} else {
						errors++;
					}
				}
			}

	/*		std::sort(patchesToTest.begin(), patchesToTest.end(), [&](const CImage& patch1, const CImage& patch2) -> bool {
				return blurMeasurer->Measure(patch1) < blurMeasurer->Measure(patch2);
			});*/

			//utils::Stack(patchesToTest, 1).Save();

			std::cout << "Patch size: " << patchSize << "x" << patchSize << std::endl <<
				"Method " << MethodNameForMethod(method) << " "
				"Errors: " << (float)errors / (float)(errors + correct) * 100 << "%" << std::endl;

			delete blurMeasurer;
		}

		std::cout << std::endl;

		delete binarizer;
		delete filter;
		delete patchFetcher;
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