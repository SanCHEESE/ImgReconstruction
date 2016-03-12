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
    
    EqualizeBrightness(normPatch1, normPatch2);
    
	CImage result;
	cv::absdiff(normPatch1, normPatch2, result);
	
    double sum = Sum(result);
    
	return sum;
}

double CImageComparator::CompareL2(const CImagePatch& patch1, const CImagePatch& patch2) const
{
    CImage normPatch1;
    patch1.GrayImage().copyTo(normPatch1);
    
    CImage normPatch2;
    patch2.GrayImage().copyTo(normPatch2);
    
    EqualizeBrightness(normPatch1, normPatch2);
    
	cv::Mat result;
	cv::absdiff(normPatch1, normPatch2, result);
	result.convertTo(result, CV_32S);
	result = result.mul(result);
	double dist = sqrt(Sum(result));
	return dist;
}

void CImageComparator::EqualizeBrightness(CImage &img1, CImage &img2) const
{
    switch (_brightnessEqualization) {
        case TBrightnessEqualizationMean:
            EqualizeBrightnessMean(img1, img2);
            break;
        case TBrightnessEqualizationDynRange:
            EqualizeBrightnessDynRange(img1, img2);
            break;
        default:
            break;
    }
}

void CImageComparator::EqualizeBrightnessMean(CImage &img1, CImage &img2) const
{
    img1.convertTo(img1, CV_16S);
    img2.convertTo(img2, CV_16S);
    
    double mean1 = cv::mean(img1)[0];
    double mean2 = cv::mean(img2)[0];
    
    double delta = mean1 - mean2;
    CImage deltaMat(img1.rows, img1.cols, CV_16S, cv::Scalar(delta));
    if (delta < 0) {
        // second image is brighter
        img1 += cv::Scalar::all(delta);
    } else {
        // first image is brighter
        img2 += cv::Scalar::all(delta);
    }
}

void CImageComparator::EqualizeBrightnessDynRange(CImage &img1, CImage &img2) const
{
    double min1, max1;
    cv::minMaxLoc(img1, &min1, &max1);
    
    double min2, max2;
    cv::minMaxLoc(img2, &min2, &max2);
    
    // https://en.wikipedia.org/wiki/Normalization_(image_processing)
    bool isFirstImage = true;
    CImage I = img1;
    double min = min1;
    double max = max1;
    double newMin = min2;
    double newMax = max2;
    if (max1 - min1 > max2 - min2) {
        // first range is bigger than second
        I = img2;
        
        min = min2;
        max = max2;
        newMin = min1;
        newMax = max1;
        isFirstImage = false;
    }
    
    for (int i = 0; i < I.rows; i++) {
        for (int j = 0; j < I.cols; j++) {
            I.at<uchar>(i, j) = (I.at<uchar>(i, j) - min) * (newMax - newMin)/(max - min) + newMin;
        }
    }
    
    if (isFirstImage) {
        img1 = I;
    } else {
        img2 = I;
    }
}

double CImageComparator::Sum(const CImage &img) const
{
    double sum = 0;
    switch (_compSum) {
        case TCompSumStd:
            sum = cv::sum(img)[0];
            break;
        case TCompSumBorder:
            for (int i = 0; i < img.rows; i++) {
                for (int j = 0; j < img.cols; j++) {
                    bool isBorderPixel = i == 0 || j == 0 || (i == img.rows - 1) || (j == img.cols - 1);
                    sum += isBorderPixel ? (_borderWeight * img.at<uchar>(i, j)) : img.at<uchar>(i, j);
                }
            }
            break;
        default:
            break;
    }
    
    return sum;
}
