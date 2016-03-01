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

void CImageProcessor::StartProcessingChain(const CImage& img, const std::string& resultImageName)
{
    _resultImageName = resultImageName;
    
    cv::Size binaryWindowSize;
    _config.GetParam(BinaryWindowSizeConfigKey).GetValue(binaryWindowSize);
    
	CImage extentImage = img.GetExtentImage(binaryWindowSize);
    
	_mainImage = CImagePatch();
	_mainImage.SetGrayImage(extentImage);
	BuildAndShowBinImage(extentImage, ENABLE_GUI);
	BuildAndShowSdImage(extentImage, ENABLE_GUI);
    
#if TEST_BLUR_METRICS || PROCESS_IMAGE || SHOW_BLUR_MAP
	WindowDidSelectPatch(_window.GetName(), {0, 0, 0, 0});
#elif ENABLE_GUI
	ConfigureWindow(extentImage);
#endif
}

void CImageProcessor::WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect)
{
#if HIGHLIGHT_SIMILAR_PATCHES
	ProcessHighlightSimilarPatches(patchRect);
#elif SHOW_SORTED_SIMILAR
	ProcessShowSortedSimilar(patchRect);
#elif REPLACE_SIMILAR_PATCHES
	ProcessReplaceSimilarPatches(patchRect);
#elif PROCESS_IMAGE
    ProcessFixImage();
#elif TEST_BLUR_METRICS
    ProcessTestBlurMetrics();
#elif SHOW_BLUR_MAP
    ProcessShowBlurMap();
#endif
}

#pragma mark - Private

void CImageProcessor::ProcessFixImage()
{
    int patchSideSize;
    TBlurMeasureMethod blurMethod;
    TAccImageSumMethod sumMethod;
    double blurParam;
    _config.GetParam(MaxPatchSideSizeConfigKey).GetValue(patchSideSize);
    _config.GetParam(BlurMeasureMethodConfigKey).GetValue(blurMethod);
    _config.GetParam(AccImageSumMethodConfigKey).GetValue(sumMethod);
    
    
    // извлечение всех патчей изображения
    std::vector<CImagePatch> patches = FetchPatches({0, 0, patchSideSize, patchSideSize});
    
    // фильтрация патчей
    patches = FilterPatches(patches);
    
    // вычисляем значение размытия
    for (CImagePatch& patch: patches) {
        if (blurMethod == TBlurMeasureMethodFFT) {
            patch.BlurValue(blurMethod, blurParam);
        } else {
            patch.BlurValue(blurMethod);
        }
    }
    
    // классификация
    std::map<uint64, std::vector<CImagePatch>> classes = Classify(patches);
    
    CAccImage accImage(_mainImage.GrayImage());
    
    int i = 0;
    for (auto &it: classes) {
        std::vector<CImagePatch> aClass = it.second;
        if (aClass.size() < 2) {
            // классы из 1 объекта не обрабатываются
            accImage.SetImageRegion(aClass[0].GrayImage());
            
            continue;
        } else {
            // ранжировка по похожести внутри класса
            auto clusters = Clusterize(aClass);

            for (auto& cluster: clusters) {
                auto clusterPatches = cluster.second;
                
                // сортировка по возрастанию размытия
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

                // копирование
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
    accImage.GetResultImage(sumMethod).Save(_resultImageName, 100, "jpg");
}
