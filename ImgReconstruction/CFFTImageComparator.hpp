#pragma once

#include <CImageComparator.hpp>
#include <CImage.h>

class CFFTImageComparator : public CImageComparator
{
public:
	using CImageComparator::CImageComparator;

	CFFTImageComparator(int eps, float radiusRatio = 0.3) : CImageComparator(0, 0, eps), _radiusRatio(radiusRatio) {};

	virtual bool Equal(const CImage& patch1, const CImage& patch2) const
	{
		float blurValue1 = BlurValue(patch1);
		float blurValue2 = BlurValue(patch2);

		return std::abs(blurValue1 - blurValue2) < _eps;
	}
private:
	float BlurValue(const CImage& img) const
	{
		CImage fft = img.GetFFTImage();
		img.CopyMetadataTo(fft);

		cv::Size submatrixSize = cv::Size((int)ceil(img.cols * _radiusRatio), (int)ceil(img.rows * _radiusRatio));
		cv::Point submatrixOrigin = cv::Point((img.cols - submatrixSize.width) / 2, (img.rows - submatrixSize.height) / 2);
		cv::Rect submatrixRect = cv::Rect(submatrixOrigin, submatrixSize);

		CImage imageCopy;
		img.copyTo(imageCopy);

		CImage roi = imageCopy(submatrixRect);
		roi.setTo(0);

		return (float)-cv::sum(imageCopy)[0];
	}

	float _radiusRatio;
};