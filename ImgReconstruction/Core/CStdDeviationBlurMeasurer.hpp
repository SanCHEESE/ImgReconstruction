//
//  CBlurStdDeviationMeasurer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "IBlurMeasurer.h"

#include "utils.h"

class CStdDeviationBlurMeasurer : public IBlurMeasurer
{
public:
	virtual double Measure(const CImage& img) const
	{
		return utils::StandartDeviation(img);
	}
};