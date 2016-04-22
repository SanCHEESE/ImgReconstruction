#pragma once

#include <cmath>
#include <vector>

#include <IImageSubprocessor.h>

class IInterpolationKernel : public IImageSubprocessor
{
public:
	virtual ~IInterpolationKernel() {};
	virtual float operator()(float x) const = 0;
	virtual std::vector<float> Coeffs(float shift) const = 0;

	virtual int A() const = 0;
};