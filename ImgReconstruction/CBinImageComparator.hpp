#pragma once


#include <CImageComparator.hpp>

#include <CImagePatch.h>
#include <CImage.h>

class CBinImageComparator : public CImageComparator
{
public:
	using CImageComparator::CImageComparator;

	virtual bool Equal(const CImagePatch& patch1, const CImagePatch& patch2) const
	{
		CImage absDiff;
		cv::absdiff(patch1.BinImage(), patch2.BinImage(), absDiff);
		return cv::sum(absDiff)[0]/255;
	}

	virtual bool Equal(const cv::cuda::GpuMat& gImg1, const cv::cuda::GpuMat& gImg2)
	{
		if (_gTemp.cols == 0 && _gTemp.rows == 0) {
			_gTemp = cuda::GpuMat(gImg1.rows, gImg1.cols, CV_32S, cv::Scalar(0));
		}

		cuda::absdiff(gImg1, gImg2, _gTemp);
		return cuda::sum(_gTemp)[0] < _eps;
	}
};