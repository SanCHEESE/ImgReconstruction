#pragma once

#include <IInterpolationKernel.h>

class CInterpolationKernel: public IInterpolationKernel
{
public:
	CInterpolationKernel(int a) : _a(a) {};

	virtual double operator()(double x) const = 0;

	virtual std::vector<double> Coeffs(double shift) const
	{
		std::vector<double> coeffs;
		double floorx = floor(shift);
		int a = this->A();
		for (int i = 0; i <= 2 * a - 1; i++) {
			coeffs.push_back((*this)(shift - (i + floorx - a + 1)));
		}

		return coeffs;
	}

	virtual int A() const
	{
		return _a;
	}
protected:
	int _a;
};
