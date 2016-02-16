//
//  CImageComparator.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageComparator.hpp"

double CImageComparator::operator()(const CImagePatch& patch1, const CImagePatch& patch2) const
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
	CImage normPatch1;
    patch1.GrayImage().copyTo(normPatch1);
    
	CImage normPatch2;
    patch2.GrayImage().copyTo(normPatch2);
    
    normPatch1.Save("normPatch1");    normPatch1.Save("normPatch2");
    
    EqualizeBrightness(normPatch1, normPatch2);
    
    normPatch1.Save("normPatch1-1");    normPatch1.Save("normPatch2-1");
    
	// вычитаем из одного другой
	CImage result;
	cv::absdiff(normPatch1, normPatch2, result);
	
	// считаем сумму по пикселям
    double sum = cv::sum(result)[0];
    
	return sum;
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

void CImageComparator::EqualizeBrightness(CImage &img1, CImage &img2) const
{
    // нормализуем первый патч
    img1.convertTo(img1, CV_16S);
    
    // нормализуем второй патч
    img2.convertTo(img2, CV_16S);
    
    double mean1 = cv::mean(img1)[0];
    double mean2 = cv::mean(img2)[0];
    
    double delta = mean1 - mean2;
    CImage deltaMat(img1.rows, img1.cols, CV_16S, cv::Scalar(delta));
    // уравнение изображений по яркости
    if (delta < 0) {
        // вторая картинка ярче
        img1 += cv::Scalar::all(delta);
    } else {
        // первая картинка ярче
        img2 += cv::Scalar::all(delta);
    }
    
    
}
