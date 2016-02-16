//
//  CAccImage.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

class CAccImage {
public:
    CAccImage(const CImage& img);
    
    void SetImage(const CImage& image);
    void SetImage(const CImage& image, const cv::Rect& frame);
    
    CImage GetResultImage(TAccImageSumMethod method) const;
private:
    static uchar Sum(TAccImageSumMethod method, std::vector<uchar> colors);
    
    std::vector<std::vector<std::vector<uchar>>> _accImg;
    cv::Size _size;
};