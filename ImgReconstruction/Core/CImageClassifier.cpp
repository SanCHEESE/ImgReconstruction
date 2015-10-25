//
//  CImageClassifier.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 22.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageClassifier.hpp"

int CImageClassifier::Classify(const CImage &image)
{
    int sideSize = image.cols/2;
    
    cv::Rect qr1 = cv::Rect(0, 0, sideSize, sideSize);
    cv::Rect qr2 = cv::Rect(sideSize, 0, image.cols - sideSize, sideSize);
    cv::Rect qr3 = cv::Rect(0, sideSize, sideSize, image.rows - sideSize);
    cv::Rect qr4 = cv::Rect(sideSize, sideSize, image.cols - sideSize, image.rows - sideSize);
    
    CImage q1(image, qr1);
    CImage q2(image, qr2);
    CImage q3(image, qr3);
    CImage q4(image, qr4);
    
    
    double mean1 = cv::mean(q1)[0];
    double mean2 = cv::mean(q2)[0];
    double mean3 = cv::mean(q3)[0];
    double mean4 = cv::mean(q4)[0];
    
    double totalMean = cv::mean(image)[0];
    
    int result = 0;
    result = mean1 > totalMean ? result : result | 1;
    result = mean2 > totalMean ? result : result | 2;
    result = mean3 > totalMean ? result : result | 4;
    result = mean4 > totalMean ? result : result | 8;
    
    return result;
}
