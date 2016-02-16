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

void CImageProcessor::StartProcessingChain(const CImage& img)
{
	CImage extentImage = img.GetExtentImage(BinaryWindowSize);
	
	_mainImage = CImagePatch();
	_mainImage.SetGrayImage(extentImage);
	BuildAndShowBinImage(extentImage, false);
	BuildAndShowSdImage(extentImage, false);
    
#if TEST_BLUR_METRICS || PROCESS_IMAGE
	WindowDidSelectPatch(_window.GetName(), {0, 0, 0, 0});
	exit(0);
#else
	ConfigureWindow(extentImage);
#endif
}

void CImageProcessor::WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect)
{
#if SHOW_BLUR_MAP
	ProcessShowBlurMap(patchRect);
#elif HIGHLIGHT_SIMILAR_PATCHES
	ProcessHighlightSimilarPatches(patchRect);
#elif SHOW_SORTED_SIMILAR
	ProcessShowSortedSimilar(patchRect);
#elif REPLACE_SIMILAR_PATCHES
	ProcessReplaceSimilarPatches(patchRect);
#elif TEST_BLUR_METRICS
	ProcessTestBlurMetrics();
#elif PROCESS_IMAGE
    ProcessFixImage();
#endif
}

#pragma mark - Private

void CImageProcessor::ProcessFixImage()
{
    // извлечение всех патчей изображения
    std::vector<CImagePatch> patches = FetchPatches({0, 0, MaxPatchSideSize, MaxPatchSideSize});
    
    // фильтрация патчей
    patches = FilterPatches(patches);
    
    // классификация
    std::map<uint64, std::vector<CImagePatch>> classes = Classify(patches);
    
    CAccImage accImage(_mainImage.GrayImage());
    for (auto &it: classes) {
        std::vector<CImagePatch> aClass = it.second;
        if (aClass.size() < 2) {
            // классы из 1 объекта не обрабатываются
            accImage.SetImage(aClass[0].GrayImage());
            continue;
        } else {
            // ранжировка по похожести внутри класса
            auto clusters = Clusterize(aClass);
            for (auto& cluster: clusters) {
                // сортировка по размытию
                std::sort(cluster.second.begin(), cluster.second.end(), MoreBlur());
                // копирование
                CImagePatch bestPatch = cluster.second[0];
                for (auto& patch: cluster.second) {
                    accImage.SetImage(bestPatch.GrayImage(), patch.GetFrame());
                }
            }
        }
    }
    
    CImage resultImage = accImage.GetResultImage(AccImageSumMethod);
    resultImage.Save();
}

#pragma mark - Utils