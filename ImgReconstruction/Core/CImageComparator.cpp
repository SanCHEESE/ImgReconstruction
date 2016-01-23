//
//  CImageComparator.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageComparator.hpp"

double CImageComparator::Compare(const CImagePatch& patch1, const CImagePatch& patch2) const
{
	switch (_compMetric) {
		case TImageCompareMetricL1:
			return CompareL1(patch1, patch2);
		case TImageCompareMetricL2:
			return CompareL2(patch1, patch2);
		default:
			break;
	}
	return DBL_MAX;
}

double CImageComparator::CompareL1(const CImagePatch& patch1, const CImagePatch& patch2) const
{
	// нормализуем первый патч
	CImage normPatch1;
	patch1.GrayImage().convertTo(normPatch1, CV_16S);
	
	// нормализуем второй патч
	CImage normPatch2;
	patch2.GrayImage().convertTo(normPatch2, CV_16S);

	double mean1 = cv::mean(normPatch1)[0];
	double mean2 = cv::mean(normPatch2)[0];
	
	double delta = mean1 - mean2;
	CImage deltaMat(normPatch1.rows, normPatch1.cols, CV_16S, cv::Scalar(delta));
	// уравнение изображений по яркости
	if (delta < 0) {
		// вторая картинка ярче
		normPatch1 += cv::Scalar::all(delta);
//		cv::add(normPatch1, deltaMat, normPatch1);
	} else {
		// первая картинка ярче
		normPatch1 += cv::Scalar::all(delta);
//		cv::add(normPatch2, deltaMat, normPatch2);
	}
	
	// вычитаем из одного другой
	CImage result;
	cv::absdiff(normPatch1, normPatch2, result);
	
	// считаем сумму по пикселям
	return sum(result)[0];
}

double CImageComparator::CompareL2(const CImagePatch& patch1, const CImagePatch& patch2) const
{
	// нормализуем патчи
	cv::Mat result;
	cv::absdiff(patch1.BinImage(), patch2.BinImage(), result);
	result.convertTo(result, CV_32S);
	result = result.mul(result);
	double dist = sqrt(sum(result)[0])/25.5;
	return dist;
}
