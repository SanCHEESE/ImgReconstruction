//
//  CAccImage.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include <common.h>
#include <CImageShifter.hpp>
#include <CImage.h>

class CAccImage
{
public:
	CAccImage(const CImage& img, const CImageShifter* const shifter = 0);
	CAccImage(const cv::Size& size, const CImageShifter* const shifter = 0);

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

		if (typeid(T) == typeid(int)) {
			setImageRegion(image, frame);
		} else if (typeid(T) == typeid(float)) {
			cv::Point2f shift(modf((float)frame.x, 0), modf((float)frame.y, 0));
			CImage shiftedImage = _shifter->ShiftImage(image, shift);
			cv::Rect2f newFrame = cv::Rect2f(floorf(frame.x), floorf(frame.y), frame.width, frame.height);
			setImageRegion(shiftedImage, newFrame);
		}
	}

	CImage GetResultImage(TAccImageSumMethod method) const;

	// debug
	CImage CreateHistImage() const;

private:
	static uchar Sum(TAccImageSumMethod method, std::vector<uchar> colors);
	const CImageShifter* const _shifter;

	std::vector<std::vector<std::vector<uchar>>> _accImg;
	cv::Size _size;
};