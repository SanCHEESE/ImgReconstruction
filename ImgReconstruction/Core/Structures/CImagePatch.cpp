//
//  CImagePatch.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 24.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImagePatch.h"

#include <bitset>

long CImagePatch::_counter = 0;

float CImagePatch::BlurValue(const IBlurMeasurer *const measurer)
{
	if (_blurValue < 0) {
		_blurValue = measurer->Measure(_grayImage, parentImage);
	}

	return _blurValue;
}

float CImagePatch::StandartDeviation()
{
	if (_standartDeviation < 0) {
		_standartDeviation = CalculateStandartDeviation();
	}

	return _standartDeviation;
}

uint64 CImagePatch::PHash()
{
	if (!_pHashComputed) {
		_pHash = utils::PHash(_grayImage);
		_pHashComputed = true;
	}
	return _pHash;
}

uint64 CImagePatch::AvgHash()
{
	if (!_avgHashComputed) {
		_avgHash = utils::AvgHash(_binImage);
		_avgHashComputed = true;
	}
	return _avgHash;
}

std::ostream& operator<<(std::ostream& os, const CImagePatch& patch)
{
	os << "Patch:\n";
	os << "\tFrame:\n\t\t" << patch.GetFrame() << std::endl;
	os << "\tBlur value:\n\t\t" << patch.GetBlurValue() << std::endl;
	os << "\tStandart deviation:\n\t\t" << patch.GetStandartDeviation() << std::endl;

	os << "\tGrey image:\n" << patch.GrayImage() << std::endl;
	os << "\tBin image:\n" << patch.BinImage() << std::endl;

	std::bitset<sizeof(uint64) * 8> phash(patch.GetPHash());
	os << "\tPHash:\n\t\t" << phash << std::endl;

	std::bitset<sizeof(uint64) * 8> avgHash(patch.GetAvgHash());
	os << "\tAvgHash:\n\t\t" << avgHash << std::endl;

	return os;
}

void CImagePatch::Initialize()
{
	aClass = -1;
	_blurValue = -1;
	_standartDeviation = -1;
	_pHashComputed = false;
	_avgHashComputed = false;
	id = _counter;

	_counter++;
}

float CImagePatch::CalculateStandartDeviation() const
{
	return utils::StandartDeviation(_grayImage);
}