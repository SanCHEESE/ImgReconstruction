#pragma once

#include "IPatchIterator.h"
#include "CImage.h"
#include "common.h"

#include "utils.h"

static const int ExtentBorderSize = 2;

template<typename T = int>
class CPatchIterator : public IPatchIterator
{
public:
	CPatchIterator(const CImage* const iterImage, const cv::Size& size, const cv::Point_<T> offset) :
		_size(size), _offset(offset), _borderInset(0)
	{
		if (typeid(T) == typeid(float)) {
			_borderInset = ExtentBorderSize;
			cv::copyMakeBorder(*iterImage, _iterImage, _borderInset, _borderInset, _borderInset, _borderInset, cv::BORDER_REFLECT, 0);
			_pointingRect = cv::Rect2f(_borderInset, _borderInset, size.width, size.height);
		} else if (typeid(T) == typeid(int)) {
			_iterImage = *iterImage;
			_pointingRect = cv::Rect_<T>(0, 0, _size.width, _size.height);
		}
	}

	virtual CImage GetNext()
	{
		float maxRow = MIN(_pointingRect.height + _pointingRect.y, _iterImage.rows - _borderInset);
		float maxCol = MIN(_pointingRect.width + _pointingRect.x, _iterImage.cols - _borderInset);

		// fetching patch
		cv::Rect_<T> patchFrame = cv::Rect_<T>(_pointingRect.x, _pointingRect.y, maxCol - _pointingRect.x, maxRow - _pointingRect.y);
		CImage patch = _iterImage(patchFrame);
		patch.SetFrame(patchFrame);

		if (_pointingRect.width + _pointingRect.x < _iterImage.cols - 1 - _borderInset) {
			// not near the right border
			_pointingRect.x += _offset.x;
		} else if (_pointingRect.width + _pointingRect.x >= _iterImage.cols - 1 - _borderInset) {
			// moving to the next row
			_pointingRect.y += _offset.y;
			_pointingRect.x = 0;
		}

		return patch;
	}

	virtual void MoveNext()
	{
		float maxRow = MIN(_pointingRect.height + _pointingRect.y, _iterImage.rows - _borderInset);
		float maxCol = MIN(_pointingRect.width + _pointingRect.x, _iterImage.cols - _borderInset);
		if (_pointingRect.width + _pointingRect.x < _iterImage.cols - 1 - _borderInset) {
			// not near the right border
			_pointingRect.x += _offset.x;
		} else if (_pointingRect.width + _pointingRect.x >= _iterImage.cols - 1 - _borderInset) {
			// moving to the next row
			_pointingRect.y += _offset.y;
			_pointingRect.x = 0;
		}
	}

	virtual bool HasNext()
	{
		if (_pointingRect.width + _pointingRect.x == _iterImage.cols - _borderInset &&
			_pointingRect.height + _pointingRect.y == _iterImage.rows - _borderInset) {
			return true;
		}

		return _pointingRect.width + _pointingRect.x + (_offset.x <= 1 ? _offset.x : (_offset.x - 1)) < _iterImage.cols - _borderInset ||
			_pointingRect.height + _pointingRect.y + (_offset.x <= 1 ? _offset.x : (_offset.x - 1)) < _iterImage.rows - _borderInset;
	}

	
	CImage GetSubRect(const CImage& image, const cv::Rect2f& rect)
	{
		double B = 0;
		double C = 0.5;

		CImage subRectImage(rect.width, rect.height, cv::DataType<uchar>::type, 0);
		for (int sr_i = 0; sr_i < rect.width; sr_i++) {
			for (int sr_j = 0; sr_j < rect.height; sr_j++) {
				// for each pixel of subsampled patch
				double x = rect.x + sr_i;
				double y = rect.y + sr_j;

				// calculate its value
				int a = 2;
				for (int i = floorf(x) - a + 1; i < floorf(x) + a; i++) {
					for (int j = floorf(y) - a + 1; j < floorf(x) + a; j++) {
						subRectImage.at<uchar>(sr_i, sr_j) += image.at<uchar>(i, j) * utils::BicubicK(x - i, B, C)*utils::BicubicK(y - j, B, C);
					}
				}
			}
		}

		return subRectImage;
	}

private:
	int _borderInset;
	cv::Size _size;
	cv::Rect_<T> _pointingRect;
	cv::Point_<T> _offset;
	CImage _iterImage;
};