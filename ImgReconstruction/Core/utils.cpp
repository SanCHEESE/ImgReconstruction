//
//  utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 15.11.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "utils.hpp"

namespace utils
{
    double StandartDeviation(const CImage& img)
    {
        cv::Scalar mean, stddev;
        cv::meanStdDev(img, mean, stddev);
        return stddev[0];
    }
    
    CImage Resize(const CImage& img, const cv::Size& size)
    {
        CImage result;
        cv::resize(img, result, size, cv::INTER_NEAREST);
        return result;
    }
    
    CImage FFT(const CImage &image)
    {
        //expand input image to optimal size
        cv::Mat padded;
        int m = cv::getOptimalDFTSize( image.rows );
        // on the border add zero values
        int n = cv::getOptimalDFTSize( image.cols );
        cv::copyMakeBorder(image, padded, 0, m - image.rows, 0, n - image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
        
        cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
        cv::Mat complexI;
        // Add to the expanded another plane with zeros
        merge(planes, 2, complexI);
        
        // this way the result may fit in the source matrix
        dft(complexI, complexI);
        
        // compute the magnitude and switch to logarithmic scale
        // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
        // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
        split(complexI, planes);
        // planes[0] = magnitude
        magnitude(planes[0], planes[1], planes[0]);
        cv::Mat magI = planes[0];
        
        // switch to logarithmic scale
        magI += cv::Scalar::all(1);
        log(magI, magI);
        
        // crop the spectrum, if it has an odd number of rows or columns
        magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));
        
        // rearrange the quadrants of Fourier image  so that the origin is at the image center
        int cx = magI.cols/2;
        int cy = magI.rows/2;
        
        // Top-Left - Create a ROI per quadrant
        cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));
        // Top-Right
        cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));
        // Bottom-Left
        cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));
        // Bottom-Right
        cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy));
        
        // swap quadrants (Top-Left with Bottom-Right)
        cv::Mat tmp;
        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);
        
        // swap quadrant (Top-Right with Bottom-Left)
        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);
        
        // Transform the matrix with float values into a
        // viewable image form (float between values 0 and 1).
        normalize(magI, magI, 0, 1, CV_MINMAX);
        
        
        return magI;
    }
    
    double MeasureBlurWithFFTImage(const CImage &image, float blurMetricRadiusRatio)
    {
        cv::Size submatrixSize = cv::Size(ceil(image.cols * blurMetricRadiusRatio), ceil(image.rows * blurMetricRadiusRatio));
        cv::Point submatrixOrigin = cv::Point((image.cols - submatrixSize.width) / 2, (image.rows - submatrixSize.height) / 2);
        cv::Rect submatrixRect = cv::Rect(submatrixOrigin, submatrixSize);
        
        CImage imageCopy;
        image.copyTo(imageCopy);
        
        CImage roi = imageCopy(submatrixRect);
        roi.setTo(0);
        
        return cv::sum(imageCopy)[0];
    }
    
    CImage SDFilter(const CImage &image, const cv::Size& filterSize)
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
    
    int64_t PHash(const CImage &image)
    {
        cv::Mat temp, dst;
        
        image.copyTo(temp);
        temp.convertTo(temp, CV_64F);
        
        cv::resize(temp, temp, cv::Size(32,32));
        cv::dct(temp, dst);
        
        double dIdex[64];
        double mean = 0.0;
        int k = 0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                dIdex[k] = dst.at<double>(i, j);
                mean += dst.at<double>(i, j) / 64;
                ++k;
            }
        }
        
        int64_t result = 0;
        for (int i = 0; i < 64; ++i) {
            if (dIdex[i] >= mean) {
                result = (result << 1) | 1;
            } else {
                result = result << 1;
            }
        }
        return result;
    }
    
    int64_t AvgHash(const CImage &image)
    {
        cv::Mat temp = image.clone();
        
        resize(temp, temp, cv::Size(8, 8));
        
        uchar *pData;
        for(int i = 0; i < temp.rows; i++) {
            pData = temp.ptr<uchar>(i);
            for(int j = 0; j < temp.cols; j++) {
                pData[j] = pData[j]/4;
            }
        }
        
        int average = cv::mean(temp)[0];
        
        cv::Mat mask = (temp >= (uchar)average);
        
        int64_t result = 0;
        for (int i = 0; i < mask.rows; i++) {
            pData = mask.ptr<uchar>(i);
            for(int j = 0; j < mask.cols; j++) {
                if (pData[j] == 0) {
                    result = result << 1;
                } else {
                    result = (result << 1) | 1;
                }
            }
        }
        return result;
    }
}
