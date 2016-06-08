//
//  utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 15.11.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "utils.h"

#include <chrono>
#include <numeric>

#include <CImage.h>
#include <common.h>

namespace utils
{
	double StandartDeviation(const CImage& img)
	{
		cv::Scalar mean, stddev;
		cv::meanStdDev(img, mean, stddev);
		return stddev[0];
	}

	uint64 PHash(const CImage &image, const cv::Size& size)
	{
		assert(image.cols <= 8 && image.rows <= 8);

		cv::Size newSize = size;
		if (size.width % 2 == 1) {
			newSize = size * 2;
		}

		cv::Mat resized;
		resize(image, resized, newSize);

		resized.convertTo(resized, CV_64F);

		cv::Mat dst;
		cv::dct(resized, dst);

		if (newSize != size) {
			resize(dst, dst, newSize / 2, (0, 0), (0, 0), cv::INTER_NEAREST);
		}

		int bitsCount = size.width * size.height;

		std::vector<double> dIdex(bitsCount, 0);
		double mean = 0.0;
		int k = 0;
		for (int i = 0; i < size.width; ++i) {
			for (int j = 0; j < size.height; ++j) {
				dIdex[k] = dst.at<double>(i, j);
				mean += dst.at<double>(i, j) / bitsCount;
				++k;
			}
		}

		uint64 result = 0;
		for (int i = 0; i < bitsCount; ++i) {
			if (dIdex[i] >= mean) {
				result = (result << 1) | 1;
			} else {
				result = result << 1;
			}
		}

		return result;
	}

	uint64 AvgHash(const CImage& image, const cv::Size& size)
	{
		assert(image.cols <= 8 && image.rows <= 8);

		cv::Mat resized;
		resize(image, resized, size);

		double average = cv::mean(resized)[0];

		cv::Mat mask = (resized > average);

		uint64 result = 0;
		for (int i = 0; i < mask.rows; i++) {
			for (int j = 0; j < mask.cols; j++) {
				if (mask.at<uchar>(i, j) == 0) {
					result = result << 1;
				} else {
					result = (result << 1) | 1;
				}
			}
		}

		return result;
	}

	std::ostream& operator<<(std::ostream& os, const cv::Mat& mat)
	{
		os << "\n";
		for (int i = 0; i < mat.rows; i++) {
			os << "\t\t\t";
			for (int j = 0; j < mat.cols; j++) {
				os << std::setw(4);
				os << (int)mat.at<uchar>(i, j) << " ";
			}
			if (i == mat.rows - 1) {
				os << "\n";
			} else {
				os << "\n\n";
			}

		}
		return os;
	}

	CImage Stack(const std::vector<CImage>& images, int cols)
	{
		//cv::Size imageSize = images[0].GetSize();
		//if (images.size() > 2) {
		//	assert(images[0].GetSize() == images[1].GetSize());
		//}

		int width = images[0].GetSize().width;
		int height = images[0].GetSize().height;

		int rows = (int)images.size() / cols;

		int resultWidth = width * cols + cols - 1;
		int resultHeight = height * rows + rows - 1;
		CImage img(resultHeight, resultWidth, CV_8UC1, cv::Scalar(255));
		int maxY = 0;
		for (int i = 0; i < images.size(); i++) {
			int col = i % cols;
			int row = i / cols;
			maxY = row * (height + 1);
			cv::Mat roi = img.colRange(col * width, (col + 1) * width).rowRange(maxY, maxY + height);
			maxY += height + 1;
			images[i].copyTo(roi);
		}

		return img;
	}

	// float Lanczos()

	float BicubicK(float x, float B, float C)
	{
		float k = 0;
		x = std::abs(x);
		if (std::abs(x) < 1) {
			k = (12 - 9 * B - 6 * C)*pow(x, 3) + (-18 + 12 * B + 6 * C)*pow(x, 2) + (6 - 2 * B);
		} else if (std::abs(x) >= 1 && std::abs(x) < 2) {
			k = (-B + 6 * C)*pow(x, 3) + (6 * B + 30 * C)*pow(x, 2) + (-12 - 48 * C)*x + (8 * B + 24 * C);
		}

		return k/6;
	}

	int LevensteinDistance(const std::string &s1, const std::string &s2)
	{
		// To change the type this function manipulates and returns, change
		// the return type and the types of the two variables below.
		int s1len = static_cast<int>(s1.size());
		int s2len = static_cast<int>(s2.size());

		auto column_start = (decltype(s1len))1;

		auto column = new decltype(s1len)[s1len + 1];
		std::iota(column + column_start, column + s1len + 1, column_start);

		for (auto x = column_start; x <= s2len; x++) {
			column[0] = x;
			auto last_diagonal = x - column_start;
			for (auto y = column_start; y <= s1len; y++) {
				auto old_diagonal = column[y];
				auto possibilities = {
					column[y] + 1,
					column[y - 1] + 1,
					last_diagonal + (s1[y - 1] == s2[x - 1] ? 0 : 1)
				};
				column[y] = std::min(possibilities);
				last_diagonal = old_diagonal;
			}
		}
		auto result = column[s1len];
		delete[] column;
		return result;
	}
}
