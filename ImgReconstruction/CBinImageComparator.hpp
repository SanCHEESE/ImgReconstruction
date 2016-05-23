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
};