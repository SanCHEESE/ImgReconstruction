#pragma once

#include <IPatchIterator.h>
#include <CImage.h>
#include <common.h>

#include <utils.h>

#include <IInterpolationKernel.h>
#include <CBicubicKernel.hpp>
#include <CLanczosCustomKernel.hpp>
#include <CLanczosKernel.hpp>

static const int ExtentBorderSize = 3;

template<typename T = int>
class CPatchIterator : public IPatchIterator
{
public:
	CPatchIterator(const CImage* const iterImage, const cv::Size& size, const cv::Point_<T> offset) :
		_size(size), _offset(offset), _borderInset(0)
	{
		if (typeid(T) == typeid(float)) {
			_borderInset = ExtentBorderSize;
			cv::copyMakeBorder(*iterImage, _iterImage, _borderInset, _borderInset, _borderInset, _borderInset, cv::BORDER_REPLICATE, 0);
			_pointingRect = cv::Rect2f(_borderInset, _borderInset, size.width, size.height);

			InitInterpKernel();
		
		} else if (typeid(T) == typeid(int)) {
			_iterImage = *iterImage;
			_pointingRect = cv::Rect_<T>(0, 0, _size.width, _size.height);
		}
	}

	~CPatchIterator()
	{
		delete _k;
	}

	void InitInterpKernel()
	{
		_a = ExtentBorderSize;

		_k = new CLanczosKernel(_a);
		_coeffsX = _k->Coeffs(_offset.x, _a);
		_coeffsY = _k->Coeffs(_offset.y, _a);

		//double sum = 0;
		//for (auto& coeff: _coeffsX) {
		//	sum += coeff;
		//}

		//std::cout << sum << std::endl;

		//sum = 0;
		//for (auto& coeff : _coeffsX) {
		//	sum += coeff;
		//}
		//std::cout << sum << std::endl;
	}

	virtual inline CImage GetNext()
	{
		float maxRow = MIN(_pointingRect.height + _pointingRect.y, _iterImage.rows - _borderInset);
		float maxCol = MIN(_pointingRect.width + _pointingRect.x, _iterImage.cols - _borderInset);

		// fetching patch
		float patchWidth = maxCol - _pointingRect.x;
		float patchHeight = maxRow - _pointingRect.y;
		cv::Rect_<T> patchFrame = cv::Rect_<T>(_pointingRect.x, _pointingRect.y, patchWidth, patchHeight);
		CImage patch;
		if (typeid(T) == typeid(float) && ((fmod(patchFrame.x, 1) != 0) || (fmod(patchFrame.y, 1) != 0))) {
			patch = GetSubRect(_iterImage, patchFrame);
			patch.interpolated = true;
		} else {
			patch = _iterImage(patchFrame);
			patch.interpolated = false;
		}
		patch.SetFrame(patchFrame);

		if (_pointingRect.width + _pointingRect.x + _offset.x <= _iterImage.cols - _borderInset) {
			// not near the right border
			_pointingRect.x += _offset.x;
		} else if (_pointingRect.width + _pointingRect.x + _offset.x > _iterImage.cols - _borderInset) {
			// moving to the next row
			_pointingRect.y += _offset.y;
			_pointingRect.x = _borderInset;
		}

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
			_pointingRect.x = _borderInset;
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
		CImage subRectImage(rect.width, rect.height, cv::DataType<uchar>::type, 0);
		for (int sr_i = 0; sr_i < rect.height; sr_i++) { // rows
			for (int sr_j = 0; sr_j < rect.width; sr_j++) { // cols
				// for each pixel of subsampled patch
				double x = rect.x + sr_j;
				double y = rect.y + sr_i;

				// calculate its value
				double p = 0;
				int i_start = floorf(y) - _a + 1;
				int j_start = floorf(x) - _a + 1;
				for (int i = i_start; i <= floorf(y) + _a; i++) { // rows
					for (int j = j_start; j <= floorf(x) + _a; j++) { // cols
						p += image.at<uchar>(i, j) * _coeffsY[i - i_start] * _coeffsX[j - j_start];
					}
				}

				subRectImage.at<uchar>(sr_i, sr_j) = p;
			}
		}

		return subRectImage;
	}

private:
	IInterpolationKernel *_k;
	int _a;
	std::vector<double> _coeffsX;
	std::vector<double> _coeffsY;

	int _borderInset;
	cv::Size _size;
	cv::Rect_<T> _pointingRect;
	cv::Point_<T> _offset;
	CImage _iterImage;
};