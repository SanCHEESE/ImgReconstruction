#pragma once

#include <common.h>
#include <IInterpolationKernel.h>
#include <CImage.h>

class CImageShifter
{
public:
	CImageShifter(IInterpolationKernel* const kernel, const CImage* const parentImage) : _interpKernel(kernel), _parentImage(parentImage) 
	{
		_a = _interpKernel->A();
	};

	virtual CImage ShiftImage(const CImage& image, const cv::Point2f& shift)
	{
		cv::Rect2f imgFrame = image.GetFrame();
		bool extented = false;
		CImage *parentImage = const_cast<CImage*>(_parentImage);
		if (imgFrame.x < _a || imgFrame.y < _a || _parentImage->rows - (imgFrame.y + imgFrame.height) < _a || _parentImage->cols - (imgFrame.x + imgFrame.width) < _a) {
			parentImage = new CImage();
			cv::copyMakeBorder(*_parentImage, *parentImage, _a, _a, _a, _a, cv::BORDER_REPLICATE, 0);
			extented = true;
		}

		_coeffsX = _interpKernel->Coeffs(shift.x);
		_coeffsY = _interpKernel->Coeffs(shift.y);

		cv::Rect2f newRect = {imgFrame.x + shift.x + extented * _a, imgFrame.y + shift.y + extented * _a, imgFrame.width, imgFrame.height};

		CImage shiftedImage = GetSubRect(*parentImage, newRect);
		shiftedImage.SetFrame({newRect.x - extented * _a, newRect.y - extented * _a, newRect.width, newRect.height});
		shiftedImage.interpolated = true;

		if (extented) {
			delete parentImage;
		}

		return shiftedImage;
	}

private:
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

	IInterpolationKernel* const _interpKernel;

	std::vector<float> _coeffsX;
	std::vector<float> _coeffsY;
	int _a;

	const CImage* const _parentImage;
};