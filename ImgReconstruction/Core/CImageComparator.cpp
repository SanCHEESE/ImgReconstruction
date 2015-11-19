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
        case TImageCompareMetricPHash:
            return ComparePHash(patch1, patch2);
        default:
            break;
    }
    return DBL_MAX;
}

double CImageComparator::CompareL1(const CImagePatch& patch1, const CImagePatch& patch2) const
{
    CImage result;
    cv::absdiff(patch1.BinImage(), patch2.BinImage(), result);
    return sum(result)[0] / 255;
}

double CImageComparator::CompareL2(const CImagePatch& patch1, const CImagePatch& patch2) const
{
    cv::Mat result;
    cv::absdiff(patch1.BinImage(), patch2.BinImage(), result);
    result.convertTo(result, CV_32S);
    result = result.mul(result);
    double dist = sqrt(sum(result)[0])/25.5;
    return dist;
}

double CImageComparator::ComparePHash(const CImagePatch& patch1, const CImagePatch& patch2) const
{
    return utils::hamming<int64_t>(patch1.GetPHash(), patch2.GetPHash());
}

double CImageComparator::CompareAvgHash(const CImagePatch& patch1, const CImagePatch& patch2) const
{
    return utils::hamming<int64_t>(patch1.GetAvgHash(), patch2.GetAvgHash());
}
