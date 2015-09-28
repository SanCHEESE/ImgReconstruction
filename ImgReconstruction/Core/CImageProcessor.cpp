//
//  CImageProcessor.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"
#include "CThreshBinarizer.hpp"
#include "CImageComparatorL1.hpp"

static const int ThresholdValue = 120;
static const int ComparisonEps = 6;
const std::string FftWindowName = "Fft";

void CImageProcessor::StartProcessingChain(const CImage& img)
{
    {
        _image = img;
        img.copyTo(_displayImage);
        CThreshBinarizer binarizer = CThreshBinarizer(ThresholdValue);
        _binarizedImage = binarizer.Binarize(_image);
    }
    
    _window.Show();
    _window.SetOriginalImage(_displayImage);
    _window.Update(_displayImage);
    _window.StartObservingMouse();
    _window.ObserveKeyboard();
}


void CImageProcessor::WindowDidSelectPatch(const CImage& img, const cv::Rect& patchRect)
{
    CImage patch = GetPatchImageFromImage(_image, patchRect);
    CImage patchFft = FFT(patch);
    
    _fftWindow = CWindow(CWindow(FftWindowName, patchFft));
    _fftWindow.Show();
    
    std::vector<CImage> patches;
    CImage::CPatchIterator patchIterator = img.GetPatchIterator(cv::Size(patchRect.width, patchRect.height), cv::Point(patchRect.width, patchRect.height));
    while (patchIterator.HasNext()) {
        patches.push_back(patchIterator.GetNext());
    }
    
    CImageComparatorL1 imgComparator = CImageComparatorL1();
    for (int i = 0; i < patches.size(); i++) {
        if (imgComparator.Compare(patch, patches[i]) < ComparisonEps) {
            _window.DrawRect(patches[i].GetFrame());
        }
    }
}


CImage CImageProcessor::GetPatchImageFromImage(const CImage &img, const cv::Rect &patchRect)
{
    return CImage(img, patchRect);
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

