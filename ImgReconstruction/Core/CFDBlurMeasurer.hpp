//
//  CFDBlurMeasurer.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IBlurMeasurer.h"

class CFDBlurMeasurer : public IBlurMeasurer
{
public:
	virtual float Measure(const CImage& img, const CImage* const parentImg = 0) const
	{
		cv::Mat padded;
		int m = cv::getOptimalDFTSize(img.rows);
		int n = cv::getOptimalDFTSize(img.cols);
		cv::copyMakeBorder(img, padded, 0, m - img.rows, 0, n - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

		cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
		cv::Mat fft;
		merge(planes, 2, fft);
		dft(fft, fft);

		// centered fft image representation
		cv::Mat Fc;
		fft.copyTo(Fc);
		Fc = Fc(cv::Rect(0, 0, Fc.cols & -2, Fc.rows & -2));
		int cx = Fc.cols / 2;
		int cy = Fc.rows / 2;

		// Top-Left - Create a ROI per quadrant
		cv::Mat q0(Fc, cv::Rect(0, 0, cx, cy));
		// Top-Right
		cv::Mat q1(Fc, cv::Rect(cx, 0, cx, cy));
		// Bottom-Left
		cv::Mat q2(Fc, cv::Rect(0, cy, cx, cy));
		// Bottom-Right
		cv::Mat q3(Fc, cv::Rect(cx, cy, cx, cy));

		// swap quadrants (Top-Left with Bottom-Right)
		cv::Mat tmp;
		q0.copyTo(tmp);
		q3.copyTo(q0);
		tmp.copyTo(q3);

		// swap quadrant (Top-Right with Bottom-Left)
		q1.copyTo(tmp);
		q2.copyTo(q1);
		tmp.copyTo(q2);

		split(Fc, planes);

		// absolute centered fft image representation values
		cv::Mat AF;
		magnitude(planes[0], planes[1], AF);

		// maximum of the value frequency component in F
		double min, max;
		cv::minMaxLoc(AF, &min, &max);

		int Th = 0;
		double thres = max / 1000;
		for (int i = 0; i < AF.rows; i++) {
			for (int j = 0; j < AF.cols; j++) {
				if (AF.at<float>(i, j) > thres) {
					Th++;
				}
			}
		}

		return Th / (float)(img.rows * img.cols);
	}
};
