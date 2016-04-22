#pragma once

#include <common.h>
#include <IInterpolationKernel.h>
#include <CImage.h>

class CImageShifter
{
public:
	CImageShifter(const IInterpolationKernel* const kernel) : _interpKernel(kernel) {};

	virtual CImage ShiftImage(const CImage& image, const cv::Point2f& shift) const
	{
		CImage extentImage;
		int a = _interpKernel->A();
		cv::copyMakeBorder(image, extentImage, a, a, a, a, cv::BORDER_REPLICATE, 0);

		auto coeffsX = _interpKernel->Coeffs(shift.x);
		auto coeffsY = _interpKernel->Coeffs(shift.y);

		CImage shiftedImage(image.cols, image.rows, cv::DataType<uchar>::type, 0);
		for (int sr_i = 0; sr_i < image.rows; sr_i++) { // rows
			for (int sr_j = 0; sr_j < image.cols; sr_j++) { // cols
				// for each pixel of subsampled patch
				float x = shift.x + sr_j;
				float y = shift.y + sr_i;

				// calculate its value
				float p = 0;
				int i_start = (int)floorf(y) - a + 1;
				int j_start = (int)floorf(x) - a + 1;
				for (int i = i_start; i <= (int)floorf(y) + a; i++) { // rows
					for (int j = j_start; j <= (int)floorf(x) + a; j++) { // cols
						p += image.at<uchar>(i, j) * coeffsY[i - i_start] * coeffsX[j - j_start];
					}
				}

				shiftedImage.at<uchar>(sr_i, sr_j) = p;
			}
		}

		return shiftedImage;
	}
private:
	const IInterpolationKernel* const _interpKernel;
};