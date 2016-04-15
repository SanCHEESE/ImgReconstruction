#pragma once

#include <CInterpolationKernel.hpp>

class CLanczosCustomKernel : public CInterpolationKernel
{
public:
	virtual double operator()(double x) const
	{
		return 0;
	}

	virtual std::vector<double> Coeffs(double shift, double a) const
	{
		return std::vector<double>({0.22954f, 0.65507f, 0.95725f, 0.95725f, 0.65507f});
	}
};