//
//  utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 15.11.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include <chrono>

#include "utils.hpp"

namespace utils
{
	double StandartDeviation(const CImage& img)
	{
		cv::Scalar mean, stddev;
		cv::meanStdDev(img, mean, stddev);
		return stddev[0];
	}
	
	uint64 PHash(const CImage &image, const cv::Size& size)
	{
		assert(image.cols <= 8 && image.rows <= 8);
		assert(size.width % 2 == 0 && size.height % 2 == 0);
		
		cv::Mat resized;
		resize(image, resized, size);
		
		resized.convertTo(resized, CV_64F);
		
		cv::Mat dst;
		cv::dct(resized, dst);
		
		int bitsCount = size.width * size.height;
		
		double dIdex[bitsCount];
		double mean = 0.0;
		int k = 0;
		for (int i = 0; i < size.width; ++i) {
			for (int j = 0; j < size.height; ++j) {
				dIdex[k] = dst.at<double>(i, j);
				mean += dst.at<double>(i, j) / bitsCount;
				++k;
			}
		}
		
		uint64 result = 0;
		for (int i = 0; i < bitsCount; ++i) {
			if (dIdex[i] >= mean) {
				result = (result << 1) | 1;
			} else {
				result = result << 1;
			}
		}
		
		return result;
	}
	
	uint64 AvgHash(const CImage& image, const cv::Size& size)
	{
		assert(image.cols <= 8 && image.rows <= 8);
		
		cv::Mat resized;
		resize(image, resized, size);
		
		int average = cv::mean(resized)[0];
		
		cv::Mat mask = (resized > average);
		
		uint64 result = 0;
		for (int i = 0; i < mask.rows; i++) {
			for (int j = 0; j < mask.cols; j++) {
				if (mask.at<uchar>(i, j) == 0) {
					result = result << 1;
				} else {
					result = (result << 1) | 1;
				}
			}
		}
		
		return result;
	}
    
    CImage CreateHistImg(const std::map<uint64, std::vector<CImagePatch>>& data)
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
        
        CImage histogramImg(maxHeight + 3 + 50, data.size() * (patchSize.width + 1) + 3, CV_8UC1, cv::Scalar(255));
        
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
            
            CImage textImg = CImage::GetImageWithText(std::to_string(patches.size()), cv::Point(0, 10), RGB(0, 0, 0), RGB(255, 255, 255), cv::Size(50, 11));
            textImg = textImg.GetRotatedImage(-90); // clockwise

            cv::Mat roi = histogramImg.rowRange(histogramImg.rows - 50 - columnImage.rows, histogramImg.rows - 50).colRange(x, x + columnImage.cols);
            columnImage.copyTo(roi);
            
            roi = histogramImg.rowRange(histogramImg.rows - 50, histogramImg.rows).colRange(x, x + textImg.cols);
            textImg.copyTo(roi);
            
            x += columnImage.cols + 1;
        }
        
        return histogramImg;
    }
	
	std::ostream& operator<<(std::ostream& os, const cv::Mat& mat)
	{
		os << "\n";
		for (int i = 0; i < mat.rows; i++) {
			os << "\t\t\t";
			for (int j = 0; j < mat.cols; j++) {
				os << std::setw(4);
				os << (int)mat.at<uchar>(i, j) << " ";
			}
			if (i == mat.rows - 1) {
				os << "\n";
			} else {
				os << "\n\n";
			}
			
		}
		return os;
	}
}
