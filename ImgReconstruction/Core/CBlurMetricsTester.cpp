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

	const int bigPatchSize = 300;
	const int bigPatchOffset = 300;
	const int minContrastValue = 30;
	const int minPatchSize = 8;
	const int maxPatchSize = 26;

	CImage img = _image({0, 0, bigPatchSize, bigPatchSize});

	/* smooth images */
	int tag = 0;
	int kernelSize = 1;
	int maxKernelSize = 27;
	std::vector<CImage> bigPatches;

	for (int idx = 0; kernelSize <= maxKernelSize; kernelSize += 2, idx++) {
		CImage tempImage;
		cv::GaussianBlur(img, tempImage, cv::Size(kernelSize, kernelSize), 0, 0);
		bigPatches.push_back(tempImage);
		tag++;
	}

	cv::Rect patchRect = {204, 200, minPatchSize, minPatchSize};

	/* iterate over patch sizes */
	for (int patchSize = minPatchSize; patchSize < maxPatchSize; patchSize++) {

		patchRect = {204, 200, patchSize, patchSize};
		std::vector<CImage> patchesToTest;

		//IBinarizer* binarizer = new CNiBlackBinarizer({patchSize, patchSize}, -0.2);
		//IPatchFilter* filter = new CPatchFilter(binarizer, minContrastValue, {patchSize / 4, patchSize / 4}, 0.5);
		//IPatchFetcher* patchFetcher = new CPatchFetcher({patchSize, patchSize}, {patchSize, patchSize}, filter);

		//CImageExtender extender({patchSize, patchSize});
		//CImage extended = extender.Extent(bigPatches[0]);
		//auto patches = patchFetcher->FetchPatches(extended);

		//patchRect = patches[7].GetFrame();

		for (CImage& bigImage : bigPatches) {
			patchesToTest.push_back(bigImage(patchRect));
		}

		utils::Stack(patchesToTest, 1).Save("orig {" + std::to_string(patchSize) + ", " + std::to_string(patchSize) + "}");

		for (TBlurMeasureMethod method = TBlurMeasureMethodStandartDeviation; method <= TBlurMeasureMethodDynamicRange; method = (TBlurMeasureMethod)((int)method + 1)) {
			IBlurMeasurer* blurMeasurer = BlurMeasurerForMethod(method);

			int errors = 0;
			int correct = 0;
			for (int i = 0; i < patchesToTest.size(); i++) {
				for (int j = 0; j < patchesToTest.size(); j++) {
					double blurValue1 = blurMeasurer->Measure(patchesToTest[i]);
					double blurValue2 = blurMeasurer->Measure(patchesToTest[j]);
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

			utils::Stack(patchesToTest, 1).Save(MethodNameForMethod(method) + " {" + std::to_string(patchSize) + ", " + std::to_string(patchSize) + "}");

			std::cout << (float)errors / (float)(errors + correct) * 100 << std::endl;

			std::cout << "Patch size: " << patchSize << "x" << patchSize << std::endl <<
				"Method " << MethodNameForMethod(method) << " "
				"Errors: " << (float)errors / (float)(errors + correct) * 100 << "%" << std::endl;

			delete blurMeasurer;
		}

		std::cout << std::endl;

		//delete binarizer;
		//delete filter;
		//delete patchFetcher;
	}
}

std::string CBlurMetricsTester::MethodNameForMethod(TBlurMeasureMethod method) const
{
	switch (method) {
		//case TBlurMeasureMethodFFT:
		//	return "TBlurMeasureMethodFFT";
		case TBlurMeasureMethodDynamicRange:
			return "TBlurMeasureMethodDynamicRange";
			//case TBlurMeasureMethodFD:
			//	return "TBlurMeasureMethodFD";
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
		//case TBlurMeasureMethodFFT:
		//	return new CFFTBlurMeasurer(0.3);
		case TBlurMeasureMethodDynamicRange:
			return new CDynamicRangeBlurMeasurer();
			//case TBlurMeasureMethodFD:
			//	return new CFDBlurMeasurer();
		case TBlurMeasureMethodStandartDeviation:
			return new CStdDeviationBlurMeasurer();
		default:
			break;
	}
	return 0;
}