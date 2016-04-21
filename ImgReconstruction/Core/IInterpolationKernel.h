#pragma once

#include <cmath>
#include <vector>

#include <IImageSubprocessor.h>

class IInterpolationKernel : public IImageSubprocessor
{
public:
	virtual ~IInterpolationKernel() {};
	virtual double operator()(double x) const = 0;
	virtual std::vector<double> Coeffs(double shift) const = 0;

	virtual int A() const = 0;
};