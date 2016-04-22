//
//  CAccImage.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include <common.h>
#include <IInterpolationKernel.h>
#include <CImageShifter.hpp>
#include <CImage.h>

class CAccImage
{
public:
	CAccImage(const CImage& img, const IInterpolationKernel* const kernel = 0);
	CAccImage(const cv::Size& size, const IInterpolationKernel* const kernel = 0);

	~CAccImage() { delete _shifter; }

	// main methods
	template<typename T>
	void SetImageRegion(const CImage& image)
	{
		SetImageRegion<T>(image, image.GetFrame());
	}

	template<typename T>
	void SetImageRegion(const CImage& image, const cv::Rect_<T>& frame)
	{
		assert(frame.x + frame.width <= _size.width);
		assert(frame.y + frame.height <= _size.height);

		auto setImageRegion = [this](const CImage& image, const cv::Rect_<T>& frame) {
			for (int y = frame.y; y < frame.y + frame.height; y++) {
				for (int x = frame.x; x < frame.x + frame.width; x++) {
					_accImg[y][x].push_back(image.at<uchar>(y - frame.y, x - frame.x));
				}
			}
		};

		float temp;
		float fractX = modf((float)frame.x, &temp);
		float fractY = modf((float)frame.y, &temp);
		if (fractY > 0 || fractX > 0) {
			cv::Point2f shift(fractX, fractY);
			CImage shiftedImage = _shifter->ShiftImage(image, shift);
			cv::Rect2f newFrame = cv::Rect2f(floorf(frame.x), floorf(frame.y), frame.width, frame.height);
			setImageRegion(shiftedImage, newFrame);
		} else {
			setImageRegion(image, frame);
		}
	}

	CImage GetResultImage(TAccImageSumMethod method) const;

	// debug
	CImage CreateHistImage() const;

private:
	static uchar Sum(TAccImageSumMethod method, std::vector<uchar> colors);
	CImageShifter* _shifter;

	std::vector<std::vector<std::vector<uchar>>> _accImg;
	cv::Size _size;
};