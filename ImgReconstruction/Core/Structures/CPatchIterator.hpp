#pragma once

#include <IPatchIterator.h>
#include <CImage.h>
#include <common.h>

#include <utils.h>

#include <IInterpolationKernel.h>
#include <CBicubicKernel.hpp>
#include <CLanczosKernel.hpp>

template<typename T = int>
class CPatchIterator : public IPatchIterator
{
public:
	CPatchIterator(const CImage* const iterImage, const cv::Size& size, const cv::Point_<T> offset, IInterpolationKernel* const k = 0) :
		_size(size), _offset(offset), _borderInset(0), _k(k), _origImage(iterImage)
	{
		if (typeid(T) == typeid(float)) {
			_borderInset = _k->A();
			_coeffsX = _k->Coeffs(_offset.x);
			_coeffsY = _k->Coeffs(_offset.y);
			_a = _k->A();

			cv::copyMakeBorder(*iterImage, _iterImage, _borderInset, _borderInset, _borderInset, _borderInset, cv::BORDER_REPLICATE, 0);
			_pointingRect = cv::Rect2f((float)_borderInset, (float)_borderInset, size.width, size.height);

		} else if (typeid(T) == typeid(int)) {
			_iterImage = *iterImage;
			_pointingRect = cv::Rect(0, 0, _size.width, _size.height);
		}
	}

	virtual inline CImage GetNext()
	{
		float maxRow = (float)MIN(_pointingRect.height + _pointingRect.y, _iterImage.rows - _borderInset);
		float maxCol = (float)MIN(_pointingRect.width + _pointingRect.x, _iterImage.cols - _borderInset);

		// fetching patch
		float patchWidth = maxCol - _pointingRect.x;
		float patchHeight = maxRow - _pointingRect.y;
		cv::Rect_<T> patchFrame = cv::Rect_<T>(_pointingRect.x, _pointingRect.y, roundf(patchWidth), roundf(patchHeight));
		CImage patch;
		if (typeid(T) == typeid(float) && ((fmod(patchFrame.x, 1) != 0) || (fmod(patchFrame.y, 1) != 0))) {
			patch = GetSubRect(_iterImage, patchFrame);
			patch.interpolated = true;
			patch.SetFrame(cv::Rect2f(patchFrame.x - _borderInset, patchFrame.y - _borderInset, patchFrame.width, patchFrame.height));
		} else {
			patch = _iterImage(patchFrame);
			patch.interpolated = false;
			patch.SetFrame(_k != 0 ? cv::Rect2f(patchFrame.x - _borderInset, patchFrame.y - _borderInset, patchFrame.width, patchFrame.height) : patchFrame);
		}

		if (_pointingRect.width + _pointingRect.x + _offset.x <= _iterImage.cols - _borderInset) {
			// not near the right border
			_pointingRect.x += _offset.x;
		} else if (_pointingRect.width + _pointingRect.x + _offset.x > _iterImage.cols - _borderInset) {
			// moving to the next row
			_pointingRect.y += _offset.y;
			_pointingRect.x = (float)_borderInset;
		}

		patch.parentImage = const_cast<CImage *>(_origImage);

		return patch;
	}

	virtual inline void MoveNext()
	{
		if (_pointingRect.width + _pointingRect.x + _offset.x <= _iterImage.cols - _borderInset) {
			// not near the right border
			_pointingRect.x += _offset.x;
		} else if (_pointingRect.width + _pointingRect.x + _offset.x > _iterImage.cols - _borderInset) {
			// moving to the next row
			_pointingRect.y += _offset.y;
			_pointingRect.x = (float)_borderInset;
		}
	}

	virtual inline bool HasNext()
	{
		if (_pointingRect.width + _pointingRect.x + _offset.x >= _iterImage.cols - _borderInset && typeid(T) == typeid(float)) {
			// if we reached end column, check if we can go down
			return _pointingRect.y + _pointingRect.height + _offset.y <= _iterImage.rows - _borderInset;
		}

		if (std::abs(_pointingRect.width + _pointingRect.x - (_iterImage.cols - _borderInset)) < _offset.x / 2 &&
			std::abs(_pointingRect.height + _pointingRect.y - (_iterImage.rows - _borderInset)) < _offset.y / 2) {
			return true;
		}

		return _pointingRect.width + _pointingRect.x + (_offset.x <= 1 ? _offset.x : (_offset.x - 1)) <= _iterImage.cols - _borderInset ||
			_pointingRect.height + _pointingRect.y + (_offset.y <= 1 ? _offset.y : (_offset.y - 1)) <= _iterImage.rows - _borderInset;
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
	const CImage * const _origImage;

	IInterpolationKernel* const _k;
	int _a;
	std::vector<float> _coeffsX;
	std::vector<float> _coeffsY;

	int _borderInset;
	cv::Size _size;
	cv::Rect_<T> _pointingRect;
	cv::Point_<T> _offset;
	CImage _iterImage;
};