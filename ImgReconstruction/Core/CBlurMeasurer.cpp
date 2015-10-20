//
//  CBlurMeasurer.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 11.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CBlurMeasurer.hpp"
#include "CImageProcessor.hpp"

double CBlurMeasurer::Measure(const CImage& img) const
{
    switch (_measureMethod) {
        case TBlurMeasureMethodStandartDeviation:
            return MeasureUsingStdDeviation(img);
        case TBlurMeasureMethodDynamicRange:
            return MeasureUsingDynamicRange(img);
        case TBlurMeasureMethodFFT:
            return MeasureUsingFFT(img);
        default:
            break;
    }
    
    return 0;
}

double CBlurMeasurer::MeasureUsingStdDeviation(const CImage &img) const
{
//    CImage image32f;
//    img.convertTo(image32f, CV_32F);
//    
//    CImage mu;
//    cv::blur(image32f, mu, img.GetFrame().size());
//    
//    CImage mu2;
//    cv::blur(image32f.mul(image32f), mu2, img.GetFrame().size());
//    
//    CImage sigma;
//    cv::sqrt(mu2 - mu.mul(mu), sigma);
//    
//    double stddev = cv::sum(sigma)[0];
    
    cv::Scalar mean, stddev;
    cv::meanStdDev(img, mean, stddev);
    
    return stddev[0]/(double)AREA(img);
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
    std::pair<std::vector<unsigned char>::iterator, std::vector<unsigned char>::iterator> minMaxElem = std::minmax_element(histArray.begin(), histArray.end());
    
    return 1 - (double)(std::abs(*(minMaxElem.first) - *(minMaxElem.second)))/(double)AREA(img);
}

double CBlurMeasurer::MeasureUsingFFT(const CImage &img) const
{
    CImage fft = CImageProcessor::FFT(img);
    return CImageProcessor::MeasureBlurWithFFTImage(fft);
}