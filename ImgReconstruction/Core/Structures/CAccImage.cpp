//
//  CAccImage.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include <numeric>
#include <CAccImage.h>


CAccImage::CAccImage(const CImage& img, IInterpolationKernel* const kernel)
{
	cv::Size size = {img.cols, img.rows};

	_accImg = std::vector<std::vector<std::vector<uchar>>>(size.height);
	for (int y = 0; y < size.height; y++) {
		_accImg[y] = std::vector<std::vector<uchar>>(size.width, std::vector<uchar>());
	}

	for (int y = 0; y < size.height; y++) {
		for (int x = 0; x < size.width; x++) {
			_accImg[y][x].push_back(img.at<uchar>(y, x));
		}
	}

	_shifter = new CImageShifter(kernel);
	_size = size;
}

CAccImage::CAccImage(const cv::Size& size, IInterpolationKernel* const kernel)
{
	_accImg = std::vector<std::vector<std::vector<uchar>>>(size.height);
	for (int y = 0; y < size.height; y++) {
		_accImg[y] = std::vector<std::vector<uchar>>(size.width, std::vector<uchar>());
	}


	_shifter = new CImageShifter(kernel);
	_size = size;
}

CImage CAccImage::GetResultImage(TAccImageSumMethod method) const
{
	CImage resultImage(_size, CV_8U, 0);
	for (int y = 0; y < _size.height; y++) {
		for (int x = 0; x < _size.width; x++) {
			auto colors = _accImg[y][x];
			resultImage.at<uchar>(y, x) = Sum(method, colors);
		}
	}

	return resultImage;
}

CImage CAccImage::CreateHistImage() const
{
	const int PixelScale = 16;

	CImage histImage(_size.height * PixelScale, _size.width * PixelScale, CV_8UC1, cv::Scalar(255));
	for (int y = 0; y < histImage.rows; y += PixelScale) {
		for (int x = 0; x < histImage.cols; x += PixelScale) {
			auto colors = _accImg[y / PixelScale][x / PixelScale];
			uchar color = Sum(TAccImageSumMethodAvg, colors);
			std::string text = std::to_string(colors.size());
			cv::Point origin = {3, 12};
			if (text.length() > 1) {
				origin = {0, 12};
			}
			CImage textWithBg = CImage::GetImageWithText(text, origin, cv::Scalar(0), cv::Scalar(color), {PixelScale, PixelScale});
			cv::Mat roi = histImage.colRange(x, x + PixelScale).rowRange(y, y + PixelScale);
			textWithBg.copyTo(roi);
		}
	}

	return histImage;
}

uchar CAccImage::Sum(TAccImageSumMethod method, std::vector<uchar> colors)
{
	uchar result;

	if (colors.size() < 1) {
		return 0;
	}

	switch (method) {
		case TAccImageSumMethodAvg:
			result = static_cast<uchar>(std::accumulate(colors.begin(), colors.end(), 0) / colors.size());
			break;
		case TAccImageSumMethodMedian:
		{
			std::sort(colors.begin(), colors.end());
			if (colors.size() % 2 == 1) {
				result = colors[colors.size() / 2];
			} else {
				result = (colors[colors.size() / 2] + colors[colors.size() / 2 - 1]) / 2;
			}
			break;
		}
		default:
			break;
	}
	return result;
}