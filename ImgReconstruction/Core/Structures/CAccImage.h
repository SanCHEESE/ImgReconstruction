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
#include <IBrightnessEqualizer.h>
#include <CImageShifter.hpp>
#include <CImage.h>

#include <utils.h>

class CAccImage
{
public:
	CAccImage(const CImage& img, IInterpolationKernel* const kernel = 0, IBrightnessEqualizer* const equalizer = 0);
	CAccImage(const cv::Size& size, IInterpolationKernel* const kernel = 0, IBrightnessEqualizer* const equalizer = 0);

	~CAccImage() { delete _shifter; }

	// main methods
	void SetImageRegion(const CImage& image, cv::Rect2f frame)
	{
		for (int y = frame.y; y < frame.y + frame.height; y++) {
			for (int x = frame.x; x < frame.x + frame.width; x++) {
				_accImg[y][x].push_back(image.at<uchar>(y - frame.y, x - frame.x));
			}
		}
	}

	void SetImageRegion(const CImage& image, const CImage& imageToCopyTo)
	{
		cv::Rect2f frame = imageToCopyTo.GetFrame();

		CImage fromImage;
		image.copyTo(fromImage);

		CImage toImage;
		imageToCopyTo.copyTo(toImage);

		_equalizer->EqualizeBrightness(fromImage, toImage);

		assert(frame.x + frame.width <= _size.width);
		assert(frame.y + frame.height <= _size.height);

		float temp;
		float fractX = modf((float)frame.x, &temp);
		float fractY = modf((float)frame.y, &temp);
		
		if (fractY > 0 || fractX > 0) {
			cv::Point2f shift(fractX, fractY);
			CImage shiftedImage = _shifter->ShiftImage(fromImage, shift);
			cv::Rect2f newFrame = cv::Rect2f(floorf(frame.x), floorf(frame.y), frame.width, frame.height);
			SetImageRegion(shiftedImage, newFrame);

		} else {
			SetImageRegion(fromImage, frame);
		}
	}

	CImage GetResultImage(TAccImageSumMethod method) const;

	// debug
	CImage CreateHistImage(TAccImageSumMethod method = TAccImageSumMethodMedian) const;

private:
	static uchar Sum(TAccImageSumMethod method, std::vector<uchar> colors);
	CImageShifter* _shifter; 
	IBrightnessEqualizer* _equalizer;

	CImage _img;

	std::vector<std::vector<std::vector<uchar>>> _accImg;
	cv::Size _size;
	
};