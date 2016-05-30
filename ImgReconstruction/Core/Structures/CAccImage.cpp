//
//  CAccImage.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include <numeric>
#include <CAccImage.h>


CAccImage::CAccImage(const CImage& img, IInterpolationKernel* const kernel, IBrightnessEqualizer* const equalizer,
	float originalWeight, float copiedWeight) : _equalizer(equalizer), _originalWeight(originalWeight), _copiedWeight(copiedWeight)
{
	cv::Size size = {img.cols, img.rows};

	_accImg = std::vector<std::vector<std::vector<CAccPixel>>>(size.height);
	for (int y = 0; y < size.height; y++) {
		_accImg[y] = std::vector<std::vector<CAccPixel>>(size.width, std::vector<CAccPixel>());
	}

	_img = img;
	_shifter = new CImageShifter(kernel, &_img);
	_size = size;
}

CImage CAccImage::GetResultImage()
{
	CImage resultImage(_size, CV_8U, 0);
	for (int y = 0; y < _size.height; y++) {
		for (int x = 0; x < _size.width; x++) {
			auto pixels = _accImg[y][x];
			if (pixels.empty()) {
				// if there is no copied patch to this pixel
				resultImage.at<uchar>(y, x) = _img.at<uchar>(y, x);
			} else {
				bool isBorder = false;
				std::vector<int> originIndices;
				for (const CAccPixel& pixel : pixels) {
					isBorder = isBorder || pixel.isBorder;
					if (pixel.isOrigin) {
						originIndices.push_back(pixel.patchIdx);
					}
				}

				if (originIndices.size() >= 1) {
					// check wether there is more origins in copied patch area
					// assume pixels has same number of cols & rows
					bool hasOrigins = false;
					CImagePatch patch = _copiedPatched[originIndices[0]];
					for (int i = y; i < y + patch.GetFrame().height; i++) {
						for (int j = x; j < x + patch.GetFrame().width; j++) {
							if (i == y && j == x) {
								continue;
							}
							for (const CAccPixel& pixel : _accImg[i][j]) {
								hasOrigins = hasOrigins || pixel.isOrigin;
							}
							if (hasOrigins) {
								break;
							}
						}
						if (hasOrigins) {
							break;
						}
					}

					if (!hasOrigins) {
						int sharpIdx = 0;
						for (int idx = 1; idx < originIndices.size(); idx++) {
							if (_copiedPatched[idx].GetBlurValue() < _copiedPatched[sharpIdx].GetBlurValue()) {
								sharpIdx = idx;
							}
						}

						for (int i = y; i < y + patch.GetFrame().height; i++) {
							for (int j = x; j < x + patch.GetFrame().width; j++) {
								for (int k = 0; k < _accImg[i][j].size(); k++) {
									if (_accImg[i][j][k].patchIdx != sharpIdx) {
										_accImg[i][j].erase(_accImg[i][j].begin() + k);
										k--;
									}
								}
							}
						}
					}


				}

				uchar accumulated = Accumulate(pixels);

				if (isBorder) {
					resultImage.at<uchar>(y, x) = (int)(_copiedWeight * accumulated + _img.at<uchar>(y, x) * _originalWeight);
				} else {
					resultImage.at<uchar>(y, x) = accumulated;

				}
			}

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
			auto pixels = _accImg[y / PixelScale][x / PixelScale];
			uchar color;
			if (pixels.empty()) {
				color = _img.at<uchar>(y / PixelScale, x / PixelScale);
			} else {
				color = Accumulate(pixels);
			}

			std::string text = std::to_string(pixels.size());
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

uchar CAccImage::Accumulate(std::vector<CAccPixel>& pixels)
{
	uchar result;

	if (pixels.size() < 1) {
		return 0;
	}

	std::sort(pixels.begin(), pixels.end());
	if (pixels.size() % 2 == 1) {
		result = pixels[pixels.size() / 2].color;
	} else {
		result = (pixels[pixels.size() / 2].color + pixels[pixels.size() / 2 - 1].color) / 2;
	}

	return result;
}