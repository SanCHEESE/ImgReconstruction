//
//  CNiBlackBinarizer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 10.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBinarizer.h"

class CDocumentBinarizer: public IBinarizer
{
public:
	CDocumentBinarizer() {_patchSize = cv::Size(25, 25);};
	CDocumentBinarizer(const cv::Size& patchSize, TBinarizationMethod binMethod = TBinarizationMethodNiBlack) :
	_patchSize(patchSize), _binMethod(binMethod), _c(2.) {};
	CDocumentBinarizer(const cv::Size& patchSize, TBinarizationMethod binMethod, double c) :
	_patchSize(patchSize), _binMethod(binMethod), _c(c) {};
	
	virtual CImage Binarize(const CImage& img) const;
public:
	cv::Size _patchSize;
	TBinarizationMethod _binMethod;
	double _c; // constant
};