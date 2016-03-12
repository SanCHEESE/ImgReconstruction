//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CTimeLogger.hpp"
#include "CDocumentBinarizer.hpp"
#include "CAccImage.hpp"

void CImageProcessor::ProcessImage(const CImage& img, const std::string& resultImageName)
{
    _resultImageName = resultImageName;
    
    GenerateHelperImages(img);
    RestoreImageIteratively(_iterCount, _mainImage.GrayImage());
}

#pragma mark - Private

void CImageProcessor::GenerateHelperImages(const CImage& img)
{
    cv::Size binaryWindowSize;
    _config.GetParam(BinaryWindowSizeConfigKey).GetValue(binaryWindowSize);
    
    CImage extentImage = img.GetExtentImage(binaryWindowSize);
    
    _mainImage = CImagePatch();
    _mainImage.SetGrayImage(extentImage);
    BuildBinImage(extentImage);
}

CImage CImageProcessor::RestoreImageIteratively(int iterCount, const CImage& img)
{
    CImage image = img;
    for (int iter = 0; iter < iterCount; iter++) {
        image = RestoreImage();
        GenerateHelperImages(image);
    }
    
    image.Save(_resultImageName, 100, "jpg");
    
    return image;
}

CImage CImageProcessor::RestoreImage()
{
    int patchSideSize;
    TBlurMeasureMethod blurMethod;
    TAccImageSumMethod sumMethod;
    double blurParam;
    _config.GetParam(MaxPatchSideSizeConfigKey).GetValue(patchSideSize);
    _config.GetParam(BlurMeasureMethodConfigKey).GetValue(blurMethod);
    _config.GetParam(AccImageSumMethodConfigKey).GetValue(sumMethod);
    
    // get all image patches
    std::vector<CImagePatch> patches = FetchPatches({0, 0, patchSideSize, patchSideSize});
    
    // filter patches
    patches = FilterPatches(patches);
    
    // calculating
    for (CImagePatch& patch: patches) {
        if (blurMethod == TBlurMeasureMethodFFT) {
            patch.BlurValue(blurMethod, blurParam);
        } else {
            patch.BlurValue(blurMethod);
        }
    }
    
    // classification by PHash/AvgHash
    std::map<uint64, std::vector<CImagePatch>> classes = Classify(patches);
    
    CAccImage accImage(_mainImage.GrayImage());
    
    int i = 0;
    for (auto &it: classes) {
        std::vector<CImagePatch> aClass = it.second;
        if (aClass.size() < 2) {
            // do not process classes with size of 1 object
            accImage.SetImageRegion(aClass[0].GrayImage());
            
            continue;
        } else {
            // ranking by sharpness inside a class
            auto clusters = Clusterize(aClass);

            for (auto& cluster: clusters) {
                auto clusterPatches = cluster.second;
                
                // sorting by blur increase
                std::sort(clusterPatches.begin(), clusterPatches.end(), MoreBlur());
                
#if IMAGE_OUTPUT_ENABLED && VERBOSE
                CImage result(1, MaxPatchSideSize, CV_8UC1, cv::Scalar(255));
                for (int i = 0; i < clusterPatches.size(); i++) {
                    CImage greyPatchImg = clusterPatches[i].GrayImage();
                    cv::Mat horisontalSeparator(1, greyPatchImg.GetFrame().width, CV_8UC1, cv::Scalar(255));
                    cv::vconcat(result, greyPatchImg, result);
                    cv::vconcat(result, horisontalSeparator, result);
                }
                
                result.Save(_resultImageName + "_" + std::to_string(i) + "_hist", 100, "jpg");
#endif
                i++;

                // copying to summing image
                CImagePatch bestPatch = clusterPatches[0];
                for (auto& patch: clusterPatches) {
                    if (patch.GetFrame() != bestPatch.GetFrame()) {
                        accImage.SetImageRegion(bestPatch.GrayImage(), patch.GetFrame());
                    }
                }
            }
        }
    }
    
#if IMAGE_OUTPUT_ENABLED
    accImage.CreateHistImage().Save(_resultImageName + "_hist_total", 100, "jpg");
#endif

    return accImage.GetResultImage(sumMethod);
}
