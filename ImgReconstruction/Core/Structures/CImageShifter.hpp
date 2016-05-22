#pragma once

#include <common.h>
#include <IInterpolationKernel.h>
#include <CImage.h>

class CImageShifter
{
public:
	CImageShifter(IInterpolationKernel* kernel) : _interpKernel(kernel) 
	{
		_a = _interpKernel->A();
	};

	virtual CImage ShiftImage(const CImage& image, const cv::Point2f& shift)
	{
		CImage* parentImage = image.parentImage;

		cv::Rect2f imgFrame = image.GetFrame();
		bool extented = false;
		if (imgFrame.x < _a || imgFrame.y < _a || parentImage->rows - (imgFrame.y + imgFrame.height) < _a || parentImage->cols - (imgFrame.x + imgFrame.width) < _a) {
			CImage *tempImage = new CImage();
			cv::copyMakeBorder(*parentImage, *tempImage, _a, _a, _a, _a, cv::BORDER_REPLICATE, 0);
			extented = true;
			parentImage = tempImage;
		}

		_coeffsX = _interpKernel->Coeffs(shift.x);
		_coeffsY = _interpKernel->Coeffs(shift.y);

		cv::Rect2f newRect = {imgFrame.x + shift.x + extented * _a, imgFrame.y + shift.y + extented * _a, imgFrame.width, imgFrame.height};

		CImage shiftedImage = GetSubRect(*parentImage, newRect);
		shiftedImage.SetFrame({newRect.x - extented * _a, newRect.y - extented * _a, newRect.width, newRect.height});
		shiftedImage.parentImage = image.parentImage;
		shiftedImage.interpolated = true;

		if (parentImage != image.parentImage) {
			delete parentImage;
		}

		return shiftedImage;
	}

	CImage GetSubRect(const CImage& image, const cv::Rect2f& rect)
	{
		int intWidth = roundf(rect.width);
		int intHeight = roundf(rect.height);
		CImage subRectImage(intWidth, intHeight, cv::DataType<uchar>::type, 0);
		for (int sr_i = 0; sr_i < intHeight; sr_i++) { // rows
			for (int sr_j = 0; sr_j < intWidth; sr_j++) { // cols
				// for each pixel of subsampled patch
				float x = rect.x + sr_j;
				float y = rect.y + sr_i;

				// calculate its value
				float p = 0;
				int i_start = (int)floor(y) - _a + 1;
				int j_start = (int)floor(x) - _a + 1;
				for (int i = i_start; i <= (int)floor(y) + _a; i++) { // rows
					for (int j = j_start; j <= (int)floor(x) + _a; j++) { // cols
						p += image.at<uchar>(i, j) * _coeffsY[i - i_start] * _coeffsX[j - j_start];
					}
				}

				subRectImage.at<uchar>(sr_i, sr_j) = p > 255 ? 255 : (p < 0 ? 0 : p);
			}
		}

		return subRectImage;
	}

private:
	std::vector<float> _coeffsX;
	std::vector<float> _coeffsY;
	int _a;

	IInterpolationKernel* _interpKernel;
};