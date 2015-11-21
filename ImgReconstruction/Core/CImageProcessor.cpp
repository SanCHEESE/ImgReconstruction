//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CTimeLogger.hpp"
#include "CDocumentBinarizer.hpp"

void CImageProcessor::StartProcessingChain(const CImage& img)
{
    _mainImage = CImagePatch();
    _mainImage.SetGrayImage(img);
    
    {
        // строим бинаризованное изображение
        CTimeLogger::StartLogging();
        
        CDocumentBinarizer binarizer(BinaryWindowSize, BinMethod, 2.f);
        CImage blurredImage;
//        cv::GaussianBlur(img, blurredImage, GaussianKernelSize, 0.2, 0.2);
        cv::bilateralFilter(img, blurredImage, 2, 1, 1);
        CImage binarizedImage;
        binarizedImage = binarizer.Binarize(blurredImage);
        _binarizedWindow = CWindow(BinarizedWindowName, binarizedImage);
        _binarizedWindow.Show();
        _binarizedWindow.Update(binarizedImage);
        
        CTimeLogger::Print("Binarization: ");
        
        _mainImage.SetBinImage(binarizedImage);
    }
    
    {
        // строим sd изображение
        CTimeLogger::StartLogging();
        
        CImage sdImage;
        sdImage = utils::SDFilter(img, cv::Size(MaxPatchSideSize, MaxPatchSideSize));
        _debugWindow.Show();
        _debugWindow.Update(sdImage);

        CTimeLogger::Print("SD filter: ");
        
        _mainImage.SetSdImage(sdImage);
    }
    
    // изображение для вывода
    img.copyTo(_displayImage);
    // делаем цветным
    cv::cvtColor(_displayImage, _displayImage, CV_GRAY2RGBA);
    
    _window.Show();
    _window.SetMaxBoxSideSize(MaxPatchSideSize);
    _window.SetOriginalImage(_displayImage);
    _window.Update(_displayImage);
    _window.StartObservingMouse();
    _window.ObserveKeyboard();
}

void CImageProcessor::WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect)
{
    cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
    cv::Point offset = cv::Point(1, 1);
    
    CImagePatch selectedPatch = CImagePatch();
    CDocumentBinarizer binarizer = CDocumentBinarizer(patchSize);
    selectedPatch.SetGrayImage(CImage(_mainImage.GrayImage(), patchRect));
    selectedPatch.SetBinImage(CImage(_mainImage.BinImage(), patchRect));
    
    selectedPatch.BlurValue(BlurMeasureMethod);
    selectedPatch.StandartDeviation();
    selectedPatch.ImgClass();
    std::cout << "-------\nSelectedPatch:\n" << selectedPatch << "\n-------" <<std::endl;

    CTimeLogger::StartLogging();
    // храним пару - gray & бинаризованное изображения
    std::vector<CImagePatch> patches;
    CImage grayImage = _mainImage.GrayImage();
    CImage binImage = _mainImage.BinImage();
    CImage sdImage = _mainImage.SdImage();
    CImage::CPatchIterator patchIterator = grayImage.GetPatchIterator(patchSize, offset);
    CImage::CPatchIterator binPatchIterator = binImage.GetPatchIterator(patchSize, offset);
    CImage::CPatchIterator sdPatchIterator = sdImage.GetPatchIterator(patchSize, offset);
    while (patchIterator.HasNext()) {
        CImagePatch imgPatch;
        imgPatch.SetBinImage(binPatchIterator.GetNext());
        imgPatch.SetGrayImage(patchIterator.GetNext());
        imgPatch.SetSdImage(sdPatchIterator.GetNext());
        patches.push_back(imgPatch);
    }
    CTimeLogger::Print("Patch fetching");

    int good = 0, bad = 0;

    CTimeLogger::StartLogging();
    std::cout << "Similar patches:\n";
    CImageComparator imgComparator(CompMetric);
    std::vector<DrawableRect> rectsToDraw;
    for (int i = 0; i < patches.size(); i++) {

        // раскомментировать для просмотра карты блюра
//        double colorComp = patches[i].BlurValue(BlurMeasureMethod);
//        cv::Scalar color = RGB(colorComp, colorComp, colorComp);
//        rectsToDraw.push_back({patches[i].GetFrame(), color, CV_FILLED});
        
        if (patches[i].ImgClass() != selectedPatch.ImgClass()) {
            continue;
        }
        
        if (CompMetric == TImageCompareMetricPHash) {
            selectedPatch.PHash();
            patches[i].PHash();
        } else if (CompMetric == TImageCompareMetricAvgHash) {
            selectedPatch.AvgHash();
            patches[i].AvgHash();
        }
        
        int distance = imgComparator.Compare(selectedPatch, patches[i]);
        int eps = CompEpsForCompMetric(CompMetric);
        if (distance < eps) {
            // чем больше размытия, тем темнее рамка вокруг патча
            cv::Scalar color = RGB(0, patches[i].BlurValue(BlurMeasureMethod), 0);
            rectsToDraw.push_back({patches[i].GetFrame(), color});
            good++;
            
            std::cout << "\t" << std::setw(4) << good << ". Frame: " << patches[i].GetFrame() << " Distance: " \
                << std::setw(3) << distance <<  " Class: " << patches[i].ImgClass() << std::endl;
        } else {
            bad++;
        }
    }
    std::cout << "\nGood patches: " << good << std::endl;
    CTimeLogger::Print("Similar patches search:");
    CTimeLogger::PrintTotalTime();
    
    _window.DrawRects(rectsToDraw);
    
    SaveImage("../../out/result.jpg", _window.GetImage());
}

#pragma mark - Utils

int CImageProcessor::CompEpsForCompMetric(TImageCompareMetric metric)
{
    switch (metric) {
        case TImageCompareMetricL1:
            return ComparisonEpsL1;
        case TImageCompareMetricL2:
            return ComparisonEpsL2;
        case TImageCompareMetricPHash:
            return ComparisonEpsPHash;
        case TImageCompareMetricAvgHash:
            return ComparisonEpsAvgHash;
        default:
            break;
    }
    
    return 0;
}


void CImageProcessor::SaveImage(const std::string path, const CImage &image)
{
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    cv::imwrite(path, image, compression_params);
}

