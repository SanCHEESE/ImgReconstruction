//
//  utils.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 26.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include <common.h>
#include <vector>
#include <map>

#include <CImage.h>
#include <CImagePatch.h>

namespace utils
{
	template<typename T>
	int hamming(T p1, T p2)
	{
		int hammingDistance = 0;
		for (int i = sizeof(T) * 8 - 1; i >= 0; i--) {
			if (((p1 >> i) % 2) != ((p2 >> i) % 2)) {
				hammingDistance++;
			}
		}

		return hammingDistance;
	}

	double StandartDeviation(const CImage& img);

	// classification
	uint64 PHash(const CImage &image, const cv::Size& size = {4, 4});
	uint64 AvgHash(const CImage &image, const cv::Size& size = {4, 4});

	float BicubicK(float x, float B, float C);

	std::ostream& operator<<(std::ostream& os, const cv::Mat& mat);

	CImage Stack(std::vector<CImage>& images, int cols);
	int LevensteinDistance(const std::string &s1, const std::string &s2);
}