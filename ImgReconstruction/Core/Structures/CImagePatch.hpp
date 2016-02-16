//
//  CImagePatch.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 24.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CBlurMeasurer.hpp"
#include "utils.hpp"

class CImagePatch
{
public:
	CImagePatch() {Initialize();}
	CImagePatch(const CImage& grayImage, const CImage& binImage, const CImage& sdImage) {
		Initialize();
		SetGrayImage(grayImage);
		SetBinImage(binImage);
		SetSdImage(sdImage);
	};
	CImagePatch(const CImagePatch& patch) {
		_frame = patch._frame;
		distanceToTarget = patch.distanceToTarget;
		_avgHash = patch._avgHash;
		_avgHashComputed = patch._avgHashComputed;
		_pHash = patch._pHash;
		_pHashComputed = patch._pHashComputed;
		_blurValue = patch._blurValue;
		_standartDeviation = patch._standartDeviation;
		SetGrayImage(patch.GrayImage());
		SetBinImage(patch.BinImage());
		SetSdImage(patch.SdImage());
	}
	
	double BlurValue(TBlurMeasureMethod method);
	double StandartDeviation();
	uint64 PHash();
	uint64 AvgHash();
	
	// accessors
	CImage GrayImage() const {return _grayImage;}
	CImage BinImage() const {return _binImage;}
	CImage SdImage() const {return _sdImage;}
	
	// setters
	void SetGrayImage(const CImage& image) {
		image.copyTo(_grayImage);
		_frame = _grayImage.GetFrame();
	};
	void SetBinImage(const CImage& image) {
		image.copyTo(_binImage);
		_frame = _binImage.GetFrame();
	};
	void SetSdImage(const CImage& image) {
		image.copyTo(_sdImage);
		_frame = _sdImage.GetFrame();
	};
	
	double GetBlurValue() const {return _blurValue;};
	double GetStandartDeviation() const {return _standartDeviation;};
	uint64 GetPHash() const {return _pHash;};
	uint64 GetAvgHash() const {return _avgHash;};
	void SetFrame(const cv::Rect& frame) {_frame = frame;};
	cv::Rect GetFrame() const {return _frame;};
    cv::Size GetSize() const {return _frame.size();};
	
	friend std::ostream& operator<<(std::ostream& os, const CImagePatch& patch);
	
	double distanceToTarget;
    double aClass;
private:
	void Initialize();
	double CalculateBlurValue(TBlurMeasureMethod method) const;
	double CalculateStandartDeviation() const;
	
	cv::Rect _frame;
	CImage _grayImage;
	CImage _binImage;
	CImage _sdImage;
	
	double _blurValue;
	double _standartDeviation;
	
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


struct LessSimilarity
{
	inline bool operator() (const CImagePatch& patch1, const CImagePatch& patch2)
	{
		return patch1.distanceToTarget < patch2.distanceToTarget;
	}
};
