#pragma once

#include <CInterpolationKernel.hpp>

class CBicubicKernel : public CInterpolationKernel
{
public:
	CBicubicKernel(int a, float b = 0.2, float c = 0.4) : CInterpolationKernel(a), _b(b), _c(c) {}

	virtual float operator()(float x) const
	{
		float k = 0;
		x = std::abs(x);
		if (x < 1) {
			k = (12 - 9 * _b - 6 * _c)*pow(x, 3) + (-18 + 12 * _b + 6 * _c)*pow(x, 2) + (6 - 2 * _b);
		} else if (x >= 1 && x < 2) {
			k = (-_b + 6 * _c)*pow(x, 3) + (6 * _b + 30 * _c)*pow(x, 2) + (-12 - 48 * _c)*x + (8 * _b + 24 * _c);
		}

		return k / 6;
	}


private:
	float _b;
	float _c;
};