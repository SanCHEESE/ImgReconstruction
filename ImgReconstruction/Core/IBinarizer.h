//
//  IBinarizer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//
#pragma once

class IBinarizer
{
public:
	virtual ~IBinarizer() {}
	virtual CImage Binarize(const CImage& img) const = 0;
};
