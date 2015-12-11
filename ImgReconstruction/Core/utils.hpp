//
//  utils.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 26.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

namespace utils
{
    template<typename T>
    int hamming(T p1, T p2)
    {
        int hammingDistance = 0;
        for (int i = sizeof(T) * 8 - 1; i >= 0; i--) {
            if (((p1 >> i) % 2) != ((p2 >> i) % 2)) {
                hammingDistance++;
            }
        }
        
        return hammingDistance;
    }
    
    // wrappers
    CImage Resize(const CImage& img, const cv::Size& size);
    double StandartDeviation(const CImage& img);
    
    // FFT
    CImage FFT(const CImage &image);
    double MeasureBlurWithFFTImage(const CImage &image, float blurMetricRadiusRatio);
    
    CImage SDFilter(const CImage &image, const cv::Size& filterSize);
    
    // classification
	uint64 PHash(const CImage &image, const cv::Size& size = {4, 4});
    uint64 AvgHash(const CImage &image, const cv::Size& size = {4, 4});

    void SaveImage(const std::string path, const CImage &image);
	
	CImage ExtentImage(const CImage& img, const cv::Size size);
}