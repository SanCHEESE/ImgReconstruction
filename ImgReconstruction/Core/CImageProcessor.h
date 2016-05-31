//
//  CImageProcessor.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CImageSubprocessorHolder.h"
#include "CImagePatch.h"

CImage CreateHistImage(const std::map<uint64, std::vector<CImagePatch>>& data);
CImage CreateHistImage(const std::map<int, std::vector<CImagePatch>>& data);

class CImageProcessor
{
public:
	CImageProcessor() : _iterCount(1)
	{
		_subprocHolder = &CImageSubprocessorHolder::GetInstance();
		_config = _subprocHolder->GetConfig();
	};

	// Project specific
	void ProcessImage(const CImage& img, const std::string& outImagePath);
	void SetIterCount(int iterCount) { _iterCount = iterCount; };

private:
	// main methods
	CImage RestoreImage();
	CImage RestoreImageIteratively(int iterCount, const CImage& img);
	void GenerateHelperImages(const CImage& img);

	// utils
	std::map<int, std::deque<CImagePatch>> Clusterize(std::unordered_set<CImagePatch, CImagePatch::hasher>& aClass);

	// misc
	std::string _outImagePath;

	CImagePatch _mainImage;
	int _iterCount;
	cv::Size _origImageSize;

	CImageSubprocessorHolder* _subprocHolder;
	CConfig _config;
};

class CGpuMat : public cuda::GpuMat
{
public:
	struct hasher
	{
		size_t operator() (const CGpuMat& mat) const
		{
			return (size_t)mat.datastart;
		}
	};

	inline bool operator==(const CGpuMat& mat2) const
	{
		return datastart == mat2.datastart;
	}
};




