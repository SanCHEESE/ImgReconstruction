//
//  CAccImage.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CAccImage.h"

#include <numeric>

CAccImage::CAccImage(const CImage& img)
{
    cv::Size size = {img.cols, img.rows};
    
    _accImg = std::vector<std::vector<std::vector<uchar>>>(size.width);
    for (int x = 0; x < size.width; x++) {
        _accImg[x] = std::vector<std::vector<uchar>>(size.height, std::vector<uchar>());
    }
    
    for (int x = 0; x < size.width; x++) {
        for (int y = 0; y < size.height; y++) {
            _accImg[x][y].push_back(img.at<uchar>(y, x));
        }
    }
    
    _size = size;
}

CAccImage::CAccImage(const cv::Size& size)
{
    _accImg = std::vector<std::vector<std::vector<uchar>>>(size.width);
    for (int x = 0; x < size.width; x++) {
        _accImg[x] = std::vector<std::vector<uchar>>(size.height, std::vector<uchar>());
    }
    
    _size = size;
}

void CAccImage::SetImageRegion(const CImage &image)
{
    SetImageRegion(image, image.GetFrame());
}

void CAccImage::SetImageRegion(const CImage& image, const cv::Rect& frame)
{
    assert(frame.x + frame.width <= _size.width);
    assert(frame.y + frame.height <= _size.height);
    for (int x = frame.x; x < frame.x + frame.width; x++) {
        for (int y = frame.y; y < frame.y + frame.height; y++) {
            _accImg[x][y].push_back(image.at<uchar>(y - frame.y, x - frame.x));
        }
    }
}

CImage CAccImage::GetResultImage(TAccImageSumMethod method) const
{
    CImage resultImage(_size, CV_8U, 0);
    for (int x = 0; x < _size.width; x++) {
        for (int y = 0; y < _size.height; y++) {
            auto colors = _accImg[x][y];
            resultImage.at<uchar>(y, x) = Sum(method, colors);
        }
    }
    
    return resultImage;
}

CImage CAccImage::CreateHistImage() const
{
    const int PixelScale = 16;
    
    CImage histImage(_size.height * PixelScale, _size.width * PixelScale, CV_8UC1, cv::Scalar(255));
    for (int x = 0; x < histImage.cols; x += PixelScale) {
        for (int y = 0; y < histImage.rows; y+= PixelScale) {
            auto colors = _accImg[x/PixelScale][y/PixelScale];
            uchar color = Sum(TAccImageSumMethodAvg, colors);
            std::string text = std::to_string(colors.size());
            cv::Point origin = {3, 12};
            if (text.length() > 1) {
                origin = {0, 12};
            }
            CImage textWithBg = CImage::GetImageWithText(text, origin, cv::Scalar(0), cv::Scalar(color), {PixelScale, PixelScale});
            cv::Mat roi = histImage.colRange(x, x + PixelScale).rowRange(y, y + PixelScale);
            textWithBg.copyTo(roi);
        }
    }
    
    return histImage;
}

uchar CAccImage::Sum(TAccImageSumMethod method, std::vector<uchar> colors)
{
    uchar result;
    
    if (colors.size() < 1) {
        return 0;
    }
    
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