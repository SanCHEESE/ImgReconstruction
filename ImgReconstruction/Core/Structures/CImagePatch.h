//
//  CImagePatch.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 24.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBlurMeasurer.h"
#include "utils.h"

#include "common.h"

class CImagePatch
{
public:
	CImagePatch() { Initialize(); }
	CImagePatch(const CImage& grayImage)
	{
		Initialize();
		SetGrayImage(grayImage);
	}
	CImagePatch(const CImage& grayImage, const CImage& binImage)
	{
		Initialize();
		SetGrayImage(grayImage);
		SetBinImage(binImage);
	};
	CImagePatch(const CImagePatch& patch)
	{
		_frame = patch._frame;
		_avgHash = patch._avgHash;
		_avgHashComputed = patch._avgHashComputed;
		_pHash = patch._pHash;
		_pHashComputed = patch._pHashComputed;
		_blurValue = patch._blurValue;
		_standartDeviation = patch._standartDeviation;
		parentImage = patch.parentImage;
		SetGrayImage(patch.GrayImage());
		SetBinImage(patch.BinImage());
	}

	float BlurValue(const IBlurMeasurer* const measurer);
	void NormalizeBlurValue(float min, float max)
	{
		_blurValue = (_blurValue - min) / (max - min);
	}
	float StandartDeviation();
	uint64 PHash();
	uint64 AvgHash();

	// accessors
	const CImage& GrayImage() const { return _grayImage; }
	const CImage& BinImage() const { return _binImage; }
	float GetBlurValue() const { return _blurValue; };
	float GetStandartDeviation() const { return _standartDeviation; };
	uint64 GetPHash() const { return _pHash; };
	uint64 GetAvgHash() const { return _avgHash; };
	cv::Rect2f GetFrame() const { return _frame; };
	cv::Size GetSize() const { return _frame.size(); };

	// setters
	void SetGrayImage(const CImage& image)
	{
		image.copyTo(_grayImage);
		_frame = _grayImage.GetFrame();
	};
	void SetBinImage(const CImage& image)
	{
		image.copyTo(_binImage);
		_frame = _binImage.GetFrame();
	};
	void SetFrame(const cv::Rect2f& frame) { _frame = frame; };

	friend std::ostream& operator<<(std::ostream& os, const CImagePatch& patch);

	int aClass;
	CImage *parentImage;
private:
	void Initialize();
	float CalculateStandartDeviation() const;

	cv::Rect2f _frame;
	CImage _grayImage;
	CImage _binImage;

	float _blurValue;
	float _standartDeviation;

	uint64 _avgHash;
	bool _avgHashComputed;
	uint64 _pHash;
	bool _pHashComputed;
};

struct MoreBlur
{
	inline bool operator() (const CImagePatch& patch1, const CImagePatch& patch2)
	{
		return patch1.GetBlurValue() > patch2.GetBlurValue();
	}
};

struct LessBlur
{
	inline bool operator() (const CImagePatch& patch1, const CImagePatch& patch2)
	{
		return patch1.GetBlurValue() < patch2.GetBlurValue();
	}
};
