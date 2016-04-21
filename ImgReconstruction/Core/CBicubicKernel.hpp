#pragma once

#include <CInterpolationKernel.hpp>

class CBicubicKernel : public CInterpolationKernel
{
public:
	CBicubicKernel(int a, double b = 0.2, double c = 0.4) : CInterpolationKernel(a), _b(b), _c(c) {}

	virtual double operator()(double x) const
	{
		double k = 0;
		x = std::abs(x);
		if (x < 1) {
			k = (12 - 9 * _b - 6 * _c)*pow(x, 3) + (-18 + 12 * _b + 6 * _c)*pow(x, 2) + (6 - 2 * _b);
		} else if (x >= 1 && x < 2) {
			k = (-_b + 6 * _c)*pow(x, 3) + (6 * _b + 30 * _c)*pow(x, 2) + (-12 - 48 * _c)*x + (8 * _b + 24 * _c);
		}

		return k / 6;
	}


private:
	double _b;
	double _c;
};