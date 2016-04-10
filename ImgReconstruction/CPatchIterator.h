#pragma once

#include "IPatchIterator.h"
#include "CImage.h"
#include "common.h"

template<typename T = int>
class CPatchIterator : public IPatchIterator
{
public:
	CPatchIterator(const CImage* const iterImage, const cv::Size& size, const cv::Point_<T> offset, const cv::Rect_<T>& pointingRect = cv::Rect_<T>()) :
		_size(size), _pointingRect(pointingRect), _offset(offset)
	{
		if (_pointingRect == cv::Rect_<T>()) {
			_pointingRect = cv::Rect_<T>(0, 0, _size.width, _size.height);
		}

		if (typeid(T) == typeid(float)) {
			cv::copyMakeBorder(*iterImage, _iterImage, 2, 2, 2, 2, cv::BORDER_REFLECT, 0);
		} else if (typeid(T) == typeid(int)) {
			_iterImage = *iterImage;
		}
	}

	virtual CImage GetNext()
	{
		int maxRow = MIN(_pointingRect.height + _pointingRect.y, _iterImage.rows);
		int maxCol = MIN(_pointingRect.width + _pointingRect.x, _iterImage.cols);

		// fetching patch
		cv::Rect_<T> patchFrame = cv::Rect_<T>(_pointingRect.x, _pointingRect.y, maxCol - _pointingRect.x, maxRow - _pointingRect.y);
		CImage patch = _iterImage(patchFrame);
		patch.SetFrame(patchFrame);

		if (_pointingRect.width + _pointingRect.x < _iterImage.cols - 1) {
			// not near the right border
			_pointingRect.x += _offset.x;
		} else if (_pointingRect.width + _pointingRect.x >= _iterImage.cols - 1) {
			// moving to the next row
			_pointingRect.y += _offset.y;
			_pointingRect.x = 0;
		}

		return patch;
	}

	virtual void MoveNext()
	{
		int maxRow = MIN(_pointingRect.height + _pointingRect.y, _iterImage.rows);
		int maxCol = MIN(_pointingRect.width + _pointingRect.x, _iterImage.cols);
		if (_pointingRect.width + _pointingRect.x < _iterImage.cols - 1) {
			// not near the right border
			_pointingRect.x += _offset.x;
		} else if (_pointingRect.width + _pointingRect.x >= _iterImage.cols - 1) {
			// moving to the next row
			_pointingRect.y += _offset.y;
			_pointingRect.x = 0;
		}
	}

	virtual bool HasNext()
	{
		if (_pointingRect.width + _pointingRect.x == _iterImage.cols &&
			_pointingRect.height + _pointingRect.y == _iterImage.rows) {
			return true;
		}

		return _pointingRect.width + _pointingRect.x + (_offset.x <= 1 ? _offset.x : (_offset.x - 1)) < _iterImage.cols ||
			_pointingRect.height + _pointingRect.y + (_offset.x <= 1 ? _offset.x : (_offset.x - 1)) < _iterImage.rows;
	}

	
private:
	cv::Size _size;
	cv::Rect_<T> _pointingRect;
	cv::Point_<T> _offset;
	CImage _iterImage;
};