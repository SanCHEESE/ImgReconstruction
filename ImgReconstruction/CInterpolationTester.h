#pragma once

#include <common.h>

#include <CImage.h>
#include <IInterpolationKernel.h>
#include <CImageShifter.hpp>

class CInterpolationTester
{
public:
	CInterpolationTester(const CImage& image) : _image(image) {};

	void Test();

	CInterpolationTester() {};
	~CInterpolationTester() {};
private:
	IInterpolationKernel* KernelForType(TInterpKernelType kernelType, int a = 2, float b = 0.8, float c = 0.2);

	IInterpolationKernel* _kernel;
	CImageShifter* _shifter;

	CImage _image;
};

