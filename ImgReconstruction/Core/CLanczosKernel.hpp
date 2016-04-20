#pragma once

#include <CInterpolationKernel.hpp>

const double PI = 3.141592653589793;

class CLanczosKernel : public CInterpolationKernel
{
public:
	CLanczosKernel(int a) : CInterpolationKernel(a) {}

	virtual double operator()(double x) const
	{
		double k = 0;
		if (x == 0) {
			k = 1;
		} else if (std::abs(x) > 0 && std::abs(x) < _a) {
			k = _a * sin(PI * x) * sin(PI * x / _a) / (PI * PI * x * x);
		}
		return k;
	}
};