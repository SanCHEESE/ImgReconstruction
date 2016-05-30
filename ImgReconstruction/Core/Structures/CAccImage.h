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

struct CAccPixel
{
	uchar color;
	int patchIdx; // patch idx that pixel belongs to
	bool isOrigin;
	bool isBorder;

	CAccPixel() {}
	CAccPixel(const CAccPixel& pixel) : color(pixel.color), patchIdx(pixel.patchIdx), isOrigin(pixel.isOrigin), isBorder(pixel.isBorder) {}

	CAccPixel& CAccPixel::operator=(const CAccPixel& pixel)
	{
		this->color = pixel.color;
		this->patchIdx = pixel.patchIdx;
		this->isOrigin = pixel.isOrigin;
		this->isBorder = pixel.isBorder;
		return *this;
	}

	bool operator < (const CAccPixel& pixel) const
	{
		return (color < pixel.color);
	}
};
class CAccImage
{
public:
	CAccImage(const CImage& img, IInterpolationKernel* const kernel = 0, IBrightnessEqualizer* const equalizer = 0,
		float originalWeight = 0.5, float copiedWeight = 0.5);

	~CAccImage() { delete _shifter; }

	// main methods

	void UnitePatches(const CImagePatch& patch, const CImagePatch& patchToCopyTo)
	{
		cv::Rect2f frame = patchToCopyTo.GetFrame();

		CImage fromImage;
		patch.GrayImage().copyTo(fromImage);

		CImage toImage;
		patchToCopyTo.GrayImage().copyTo(toImage);

		assert(frame.x + frame.width <= _size.width);
		assert(frame.y + frame.height <= _size.height);

		float temp;
		float fractX = modf((float)frame.x, &temp);
		float fractY = modf((float)frame.y, &temp);

		_copiedPatched.push_back(patch);
		
		if (fractY > 0 || fractX > 0) {
			cv::Point2f shift(fractX, fractY);
			CImage shiftedImage = _shifter->ShiftImage(fromImage, shift);
			// cast fromImage brightness to brightness of combined image
			_equalizer->EqualizeBrightness(shiftedImage, toImage);
			cv::Rect2f newFrame = cv::Rect2f(floorf(frame.x), floorf(frame.y), frame.width, frame.height);
			CopyImageToFrame(shiftedImage, newFrame);
		} else {
			_equalizer->EqualizeBrightness(fromImage, toImage);
			CopyImageToFrame(fromImage, frame);
		}
	}

	CImage GetResultImage();

	// debug
	CImage CreateHistImage() const;

private:
	void CopyImageToFrame(const CImage& image, cv::Rect frame)
	{
		for (int y = frame.y; y < frame.y + frame.height; y++) {
			for (int x = frame.x; x < frame.x + frame.width; x++) {
				CAccPixel pixel;
				pixel.isOrigin = y == frame.y && x == frame.x;
				pixel.color = image.at<uchar>(y - frame.y, x - frame.x);
				pixel.isBorder = y == frame.y + frame.height - 1 ||
					x == frame.x + frame.width - 1 ||
					y == frame.y || x == frame.x;
				// last copied patch is img
				pixel.patchIdx = (int)_copiedPatched.size() - 1;
				_accImg[y][x].push_back(pixel);
			}
		}
	}
	static uchar Accumulate(std::vector<CAccPixel>& pixels);
	CImageShifter* _shifter; 
	IBrightnessEqualizer* _equalizer;

	CImage _img;
	cv::Size _size;

	std::vector<std::vector<std::vector<CAccPixel>>> _accImg;
	std::vector<CImagePatch> _copiedPatched;

	float _originalWeight;
	float _copiedWeight;
};