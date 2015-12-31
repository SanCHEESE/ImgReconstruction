//
//  IMeasurer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 11.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

class IMeasurer
{
public:
	virtual ~IMeasurer() {}
	virtual double Measure(const CImage& img) const = 0;
};