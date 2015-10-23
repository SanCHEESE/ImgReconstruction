//
//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CDocumentBinarizer.hpp"
#include "CImageComparatorL1.hpp"
#include "CBlurMeasurer.hpp"
#include "CTimeLogger.hpp"

static const int ProgressCount = 1000;
static const int ComparisonEps = 10;
static const int MaxPatchSideSize = 10;
static const float BlurMetricRadiusRatio = 0.2f;

const std::string DebugWindowName = "Debug";
const std::string BinarizedWindowName = "Binarized";

struct ImagePatch {
    CImage grayImage;
    CImage binImage;
    CImage sdImage;
    int imgClass;
};

struct Greater
{
    inline bool operator() (const ImagePatch& imgPair1, const ImagePatch& imgPair2)
    {
        return imgPair1.grayImage.GetBlurValue() > imgPair2.grayImage.GetBlurValue();
    }
};

void CImageProcessor::StartProcessingChain(const CImage& img)
{
    {
        // серое изображение
        _image = img;
        
        // изображение для вывода
        img.copyTo(_displayImage);
        // делаем цветным
        cv::cvtColor(_displayImage, _displayImage, CV_GRAY2RGBA);
        
        CTimeLogger::StartLogging();
        // показываем бинаризованное изображение
        CDocumentBinarizer binarizer(cv::Size(25, 25));
        _binarizedImage = binarizer.Binarize(_image);
        CTimeLogger::Print("Binarization: ");
        
        _binarizedWindow = CWindow(BinarizedWindowName, _binarizedImage);
        _binarizedWindow.Show();
        _binarizedWindow.Update(_binarizedImage);
    }
    
    {
        CTimeLogger::StartLogging();
        _sdImage = SDFilter(_image, cv::Size(MaxPatchSideSize, MaxPatchSideSize));
        _debugWindow.Show();
        _debugWindow.Update(_sdImage);
        CTimeLogger::Print("SD filter: ");

    }
    
    _window.Show();
    _window.SetMaxBoxSideSize(MaxPatchSideSize);
    _window.SetOriginalImage(_displayImage);
    _window.Update(_displayImage);
    _window.StartObservingMouse();
    _window.ObserveKeyboard();
}

void CImageProcessor::WindowDidSelectPatch(const CImage& img, const cv::Rect& patchRect)
{
    CImage patch = GetPatchImageFromImage(_image, patchRect);
    
    std::cout << "-------\nSelected patch: " << patch.GetFrame() << "\n-------" << std::endl;
    
    cv::Size patchSize = cv::Size(patchRect.width, patchRect.height);
    cv::Point offset = cv::Point(1, 1);
    
    CTimeLogger::StartLogging();
    // храним пару - gray & бинаризованное изображения
    std::vector<ImagePatch> patches;
    CImage::CPatchIterator patchIterator = _image.GetPatchIterator(patchSize, offset);
    CImage::CPatchIterator binPatchIterator = _binarizedImage.GetPatchIterator(patchSize, offset);
    CImage::CPatchIterator sdPatchIterator = _sdImage.GetPatchIterator(patchSize, offset);
    while (patchIterator.HasNext()) {
        ImagePatch imgPatch;
        imgPatch.grayImage = patchIterator.GetNext();
        imgPatch.binImage = binPatchIterator.GetNext();
        imgPatch.sdImage = sdPatchIterator.GetNext();
        imgPatch.grayImage.CalculateBlurValue(TBlurMeasureMethodStandartDeviation);
        imgPatch.grayImage.CopyMetadataTo(imgPatch.binImage);
        imgPatch.grayImage.CopyMetadataTo(imgPatch.sdImage);
        patches.push_back(imgPatch);
    }
    CTimeLogger::Print("Patch fetching & blur calculation:");
    
    // бинаризуем выбранный патч
    CDocumentBinarizer binarizer = CDocumentBinarizer(patchSize);
    CImage binarized_patch = GetPatchImageFromImage(_binarizedImage, patchRect);

    int good = 0, bad = 0;

    CTimeLogger::StartLogging();
    CImageComparatorL1 imgComparator;
    std::vector<DrawableRect> rectsToDraw;
    for (int i = 0; i < patches.size(); i++) {

        // раскомментировать для просмотра тепловой карты блюра
//        double colorComp = patches[i].grayImage.GetBlurValue() * 255;
//        cv::Scalar color = RGB(colorComp, colorComp, colorComp);
//        rectsToDraw.push_back({patches[i].grayImage.GetFrame(), i == 0 ? RGB(0, 255, 0) : color, CV_FILLED});
        
        if (imgComparator.Compare(binarized_patch, patches[i].binImage) < ComparisonEps) {
            // чем больше размытия, тем темнее рамка вокруг патча
            cv::Scalar color = RGB(0, patches[i].grayImage.GetBlurValue() * 255, 0);
            // похожий самый четкий патч выделяем таким же цветом
            rectsToDraw.push_back({patches[i].grayImage.GetFrame(), i == 0 ? RGB(0, 255, 0) : color});
            good++;
        } else {
            bad++;
        }
        
//        if (i % ProgressCount == 0) {
//            std::cout << "Progress: " << (float)i/(float)patches.size() * 100 << "%\n\tGood: " << good << "\n\tBad: " << bad << std::endl;
//        }
    }
    CTimeLogger::Print("Finding similar patches:");
    
    _window.DrawRects(rectsToDraw);
    
    SaveImage("../../output/result.jpg", _window.GetImage());
}

#pragma mark - Utils

CImage CImageProcessor::GetPatchImageFromImage(const CImage &img, const cv::Rect &patchRect)
{
    return CImage(img, patchRect);
}

void CImageProcessor::SaveImage(const std::string path, const CImage &image)
{
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    cv::imwrite(path, image, compression_params);
}

#pragma mark - Algorithms

CImage CImageProcessor::SDFilter(const CImage &image, const cv::Size& filterSize)
{
    CImage image32f;
    image.convertTo(image32f, CV_32F);
    
    CImage mu;
    blur(image32f, mu, filterSize);
    
    CImage mu2;
    blur(image32f.mul(image32f), mu2, filterSize);
    
    CImage sigma;
    cv::sqrt(mu2 - mu.mul(mu), sigma);
    
    return CImage(sigma / 255.f);
}

#pragma mark - Not used

double CImageProcessor::MeasureBlurWithFFTImage(const CImage &image)
{
    cv::Size submatrixSize = cv::Size(ceil(image.cols * BlurMetricRadiusRatio), ceil(image.rows * BlurMetricRadiusRatio));
    cv::Point submatrixOrigin = cv::Point((image.cols - submatrixSize.width) / 2, (image.rows - submatrixSize.height) / 2);
    cv::Rect submatrixRect = cv::Rect(submatrixOrigin, submatrixSize);
    
    CImage imageCopy;
    image.copyTo(imageCopy);
    
    CImage roi = imageCopy(submatrixRect);
    roi.setTo(0);
    
    double normalizedValue = cv::sum(imageCopy)[0]/AREA(image);
    // value from 0 to 1
    return normalizedValue;
}

CImage CImageProcessor::FFT(const CImage &image)
{
    cv::Mat padded;                            //expand input image to optimal size
    int m = cv::getOptimalDFTSize( image.rows );
    int n = cv::getOptimalDFTSize( image.cols ); // on the border add zero values
    cv::copyMakeBorder(image, padded, 0, m - image.rows, 0, n - image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
    
    dft(complexI, complexI);            // this way the result may fit in the source matrix
    
    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    cv::Mat magI = planes[0];
    
    magI += cv::Scalar::all(1);                    // switch to logarithmic scale
    log(magI, magI);
    
    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));
    
    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols/2;
    int cy = magI.rows/2;
    
    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right
    
    cv::Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    
    q1.copyTo(tmp);                         // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
    
    normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
                                            // viewable image form (float between values 0 and 1).
    
    return magI;
}

