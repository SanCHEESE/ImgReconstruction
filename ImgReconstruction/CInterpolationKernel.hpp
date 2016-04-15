#pragma once

#include <IInterpolationKernel.h>

class CInterpolationKernel: public IInterpolationKernel
{
public:
	virtual double operator()(double x) const = 0;

	virtual std::vector<double> Coeffs(double shift, int a) const
	{
		std::vector<double> coeffs;
		double floorx = floorf(shift);
		for (int i = 0; i <= 2 * a - 1; i++) {
			coeffs.push_back((*this)(shift - (i + floorx - a + 1)));
		}

		return coeffs;
	}
};
