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
    int hamming(T p1, T p2) {
        T diff = p1 & p2;
        
        std::bitset<sizeof(T) * 8> d(diff);
        
        int hammingDistance = 0;
        for (int i = 0; i < d.size(); i++) {
            hammingDistance += d[i];
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
    
    //
    int64_t PHash(const CImage &image);
    int64_t AvgHash(const CImage &image);
}