//
//  CBlurMeasurer.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 11.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CBlurMeasurer.h"
#include "CImageProcessor.h"

double CBlurMeasurer::Measure(const CImage& img) const
{
    return CBlurMeasurer::Measure(img, 0);
}

double CBlurMeasurer::Measure(const CImage& img, double radiusRatio) const
{
    switch (_measureMethod) {
        case TBlurMeasureMethodStandartDeviation:
            return MeasureUsingStdDeviation(img);
        case TBlurMeasureMethodDynamicRange:
            return MeasureUsingDynamicRange(img);
        case TBlurMeasureMethodFFT:
            return MeasureUsingFFT(img, radiusRatio);
        case TBlurMeasureMethodFD:
            return MeasureUsingFD(img);
        default:
            break;
    }
    
    return 0;
}

double CBlurMeasurer::MeasureUsingStdDeviation(const CImage &img) const
{
	return utils::StandartDeviation(img);
}

double CBlurMeasurer::MeasureUsingDynamicRange(const CImage &img) const
{
	int histSize = 256;
	float range[] = {0, 256} ;
	const float* histRange = {range};
	
	cv::Mat histogram;
	cv::calcHist( &img, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);
	
	std::vector<unsigned char> histArray;
	histogram.col(0).copyTo(histArray);
	auto minMaxElem = std::minmax_element(histArray.begin(), histArray.end());
	
	return std::abs(*(minMaxElem.first) - *(minMaxElem.second));
}

double CBlurMeasurer::MeasureUsingFFT(const CImage &img, double radiusRatio) const
{
	CImage fft = img.GetFFTImage();
	img.CopyMetadataTo(fft);
    
	cv::Size submatrixSize = cv::Size(ceil(img.cols * radiusRatio), ceil(img.rows * radiusRatio));
	cv::Point submatrixOrigin = cv::Point((img.cols - submatrixSize.width) / 2, (img.rows - submatrixSize.height) / 2);
	cv::Rect submatrixRect = cv::Rect(submatrixOrigin, submatrixSize);
	
	CImage imageCopy;
	img.copyTo(imageCopy);
	
	CImage roi = imageCopy(submatrixRect);
	roi.setTo(0);
	
	return cv::sum(imageCopy)[0];
}

double CBlurMeasurer::MeasureUsingFD(const CImage &img) const
{
    cv::Mat padded;
    int m = cv::getOptimalDFTSize( img.rows );
    int n = cv::getOptimalDFTSize( img.cols );
    cv::copyMakeBorder(img, padded, 0, m - img.rows, 0, n - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat fft;
    merge(planes, 2, fft);
    dft(fft, fft);
    
    // centered fft image representation
    cv::Mat Fc;
    fft.copyTo(Fc);
    Fc = Fc(cv::Rect(0, 0, Fc.cols & -2, Fc.rows & -2));
    int cx = Fc.cols/2;
    int cy = Fc.rows/2;
    
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
    
    return Th/(double)(img.rows * img.cols);
}