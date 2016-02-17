//
//  utils.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 26.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "CImagePatch.hpp"

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
	
	double StandartDeviation(const CImage& img);
	
	// classification
	uint64 PHash(const CImage &image, const cv::Size& size = {4, 4});
	uint64 AvgHash(const CImage &image, const cv::Size& size = {4, 4});
    
    template<typename Key>
    CImage CreateHistImg(const std::map<Key, std::vector<CImagePatch>>& data)
    {
        cv::Size patchSize = data.begin()->second[0].GetSize();
        
        int maxHeight = 0;
        // count height
        for (auto& it: data) {
            int rowHeight = it.second.size() * (patchSize.height + 1);
            if (maxHeight < rowHeight) {
                maxHeight = rowHeight;
            }
        }
        
        CImage histogramImg(maxHeight + 3 + 50, data.size() * (patchSize.width + 3) + 3, CV_8UC1, cv::Scalar(255));
        
        int x = 0;
        for (auto& it: data) {
            auto patches = it.second;
            
            CImage columnImage(1, MaxPatchSideSize, CV_8UC1, cv::Scalar(255));
            for (int i = 0; i < patches.size(); i++) {
                CImage greyPatchImg = patches[i].GrayImage();
                cv::Mat horisontalSeparator(1, greyPatchImg.GetFrame().width, CV_8UC1, cv::Scalar(255));
                cv::vconcat(columnImage, greyPatchImg, columnImage);
                cv::vconcat(columnImage, horisontalSeparator, columnImage);
            }
            
            CImage textImg = CImage::GetImageWithText(std::to_string(patches.size()), cv::Point(1, 10), RGB(0, 0, 0), RGB(255, 255, 255), cv::Size(50, 11));
            textImg = textImg.GetRotatedImage(-90); // clockwise
            
            cv::Mat roi = histogramImg.rowRange(histogramImg.rows - 50 - columnImage.rows, histogramImg.rows - 50).colRange(x, x + columnImage.cols);
            columnImage.copyTo(roi);
            
            cv::Mat roi2 = histogramImg.rowRange(histogramImg.rows - 50, histogramImg.rows).colRange(x, x + textImg.cols);
            textImg.copyTo(roi2);
            
            x += columnImage.cols + 3;
        }
        
        return histogramImg;
    }
	
	std::ostream& operator<<(std::ostream& os, const cv::Mat& mat);
}