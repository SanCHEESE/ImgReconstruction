#pragma once

#include "CImage.h"

#include <IBlurMeasurer.h>

class CBlurMetricsTester
{
public:
	CBlurMetricsTester(const CImage& image) : _image(image)
	{
	};

	void Test();

private:
	IBlurMeasurer* BlurMeasurerForMethod(TBlurMeasureMethod type) const;
	std::string MethodNameForMethod(TBlurMeasureMethod type) const;

	CImage _image;
};