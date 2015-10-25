//
//  CDocumentBinarizer.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 10.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CDocumentBinarizer.hpp"
#include "CImageProcessor.hpp"

static double const k = -0.2f;

CImage CDocumentBinarizer::Binarize(const CImage &img) const
{
    std::vector<CImage> imgPatches = img.GetAllPatches(_patchSize, cv::Point(_patchSize.width, _patchSize.height));
    std::vector<CImage> binarizedPatches = std::vector<CImage>(imgPatches.size());
    for (int i = 0; i < imgPatches.size(); i++) {
        CImage patch = imgPatches[i];

        patch.convertTo(patch, CV_64F);

        double mean = cv::mean(patch)[0];
        CImage meanMat = CImage(patch.rows, patch.cols, CV_64F, ceil(mean));
        
        CImage covarMat;
        cv::absdiff(meanMat, patch, covarMat);
        
        double thresholdValue;
        switch (_binMethod) {
            case TBinarizationMethodNICK:
            {
                double pixelsSumOfSquares = cv::sum(patch.mul(patch))[0];
                thresholdValue = mean + k * sqrt((pixelsSumOfSquares - mean)/patch.GetFrame().area());
                break;
            }
            case TBinarizationMethodNiBlack:
                thresholdValue = mean + k * CImageProcessor::StandartDeviation(img) - 10;
                break;
            default:
                break;
        }
        
        CImage binarizedPatch;
        patch.convertTo(patch, CV_8U);
        cv::threshold(patch, binarizedPatch, thresholdValue, 255, cv::THRESH_BINARY);

        patch.CopyMetadataTo(binarizedPatch);
        binarizedPatches[i] = binarizedPatch;
    }
    
    CImage resultImg(img.rows, img.cols, CV_8U, cv::Scalar(0));
    for (int i = 0; i < binarizedPatches.size(); i++) {
        CImage tmp = resultImg(cv::Rect(binarizedPatches[i].GetFrame()));
        binarizedPatches[i].copyTo(tmp);
    }
    
    return resultImg;
}