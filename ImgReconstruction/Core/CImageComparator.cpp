//
//  CImageComparator.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageComparator.hpp"

double CImageComparator::Compare(const CImage& img1, const CImage& img2) const
{
    switch (_compMetric) {
        case TImageCompareMetricL1:
            return CompareL1(img1, img2);
        case TImageCompareMetricL2:
            return CompareL2(img1, img2);
        case TImageCompareMetricDCT:
            return CompareDCT(img1, img2);
        default:
            break;
    }
    return DBL_MAX;
}

double CImageComparator::CompareL1(const CImage &img1, const CImage &img2) const
{
    CImage result;
    cv::absdiff(img1, img2, result);
    return sum(result)[0] / 255;
}

double CImageComparator::CompareL2(const CImage &img1, const CImage &img2) const
{
    cv::Mat result;
    cv::absdiff(img1, img2, result);
    result.convertTo(result, CV_32S);
    result = result.mul(result);
    double dist = sqrt(sum(result)[0])/25.5;
    return dist;
}

double CImageComparator::CompareDCT(const CImage &img1, const CImage &img2) const
{
    return 0;
}
