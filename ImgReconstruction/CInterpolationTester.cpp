#include "CInterpolationTester.h"

#include <CLanczosKernel.hpp>
#include <CBicubicKernel.hpp>

#include <utils.h>

void CInterpolationTester::Test()
{
	float shifts[] = {0.2, 0.25, 0.5};

	for (TInterpKernelType i = TInterpKernelBicubic; i <= TInterpKernelLanczos; i = (TInterpKernelType)((int)i + 1)) {
		_kernel = KernelForType(i);

		_shifter = new CImageShifter(_kernel);

		std::vector<CImage> images;
		for (int j = 0; j < (sizeof(shifts) / sizeof(*shifts)); j++) {
			CImage temp = _image;
			images.push_back(_image);
			cv::Point2f shift(shifts[j], shifts[j]);
			for (int k = 0; k < 30; k++) {
				temp = _shifter->ShiftImage(temp, shift);
				temp = _shifter->ShiftImage(temp, -shift);
				images.push_back(temp);
			}

			std::stringstream stream;
			stream << i << " " << shifts[j];
			utils::Stack(images, 1).Save(stream.str());
			images.clear();
		}


		delete _shifter;
		delete _kernel;
	}
}

IInterpolationKernel* CInterpolationTester::KernelForType(TInterpKernelType kernelType, int a, float b, float c)
{
	switch (kernelType) {
		case TInterpKernelBicubic:
			return new CBicubicKernel(a, b, c);
		case TInterpKernelLanczos:
			return new CLanczosKernel(a);
		default:
			break;
	}

	return 0;
}