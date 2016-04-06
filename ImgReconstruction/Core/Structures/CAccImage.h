//
//  CAccImage.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "common.h"

#include "CImage.h"

class CAccImage
{
public:
	CAccImage(const CImage& img);
	CAccImage(const cv::Size& size);

	// main methods
	void SetImageRegion(const CImage& image);
	void SetImageRegion(const CImage& image, const cv::Rect& frame);
	CImage GetResultImage(TAccImageSumMethod method) const;

	// debug
	CImage CreateHistImage() const;

private:
	static uchar Sum(TAccImageSumMethod method, std::vector<uchar> colors);

	std::vector<std::vector<std::vector<uchar>>> _accImg;
	cv::Size _size;
};