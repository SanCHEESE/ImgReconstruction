//
//  CAccImage.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CAccImage.hpp"

#include <numeric>

CAccImage::CAccImage(const CImage& img)
{
    cv::Size size = {img.cols, img.rows};
    
    _accImg = std::vector<std::vector<std::vector<uchar>>>(size.width);
    for (int x = 0; x < _accImg.size(); x++) {
        _accImg[x] = std::vector<std::vector<uchar>>(size.height, std::vector<uchar>());
    }
    
    for (int x = 0; x < size.width; x++) {
        for (int y = 0; y < size.height; y++) {
            _accImg[x][y].push_back(img.at<uchar>(x, y));
        }
    }
    
    _size = size;
}

void CAccImage::SetImage(const CImage &image)
{
    SetImage(image, image.GetFrame());
}

void CAccImage::SetImage(const CImage& image, const cv::Rect& frame)
{
    assert(frame.x + frame.width <= _size.width);
    assert(frame.y + frame.height <= _size.height);
    for (int x = frame.x; x < frame.x + frame.width; x++) {
        for (int y = frame.y; y < frame.y + frame.height; y++) {
            _accImg[x][y].push_back(image.at<uchar>(x - frame.x, y - frame.y));
        }
    }
}

CImage CAccImage::GetResultImage(TAccImageSumMethod method) const
{
    CImage resultImage(_size, CV_8U, 0);
    for (int x = 0; x < _size.width; x++) {
        for (int y = 0; y < _size.height; y++) {
            auto colors = _accImg[x][y];
            resultImage.at<uchar>(x, y) = Sum(method, colors);
        }
    }
    
    return resultImage;
}

uchar CAccImage::Sum(TAccImageSumMethod method, std::vector<uchar> colors)
{
    uchar result;
    
    switch (method) {
        case TAccImageSumMethodAvg:
            result = std::accumulate(colors.begin(), colors.end(), 0)/colors.size();
            break;
        case TAccImageSumMethodMedian:
        {
            std::sort(colors.begin(), colors.end());
            if (colors.size() % 2 == 1) {
                result = colors[colors.size()/2];
            } else {
                result = (colors[colors.size()/2] + colors[colors.size()/2 - 1])/2;
            }
            break;
        }
        default:
            break;
    }
    return result;
}