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
    
    void SaveImage(const std::string path, const CImage &image)
    {
        std::vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);
        cv::imwrite(path, image, compression_params);
    }
	
	CImage ExtentImage(const CImage& img, const cv::Size size)
	{
		cv::Size iterImageSize = {0, 0};
		if (img.cols % size.width > 0) {
			iterImageSize.width = (img.cols / size.width + 1) * size.width;
		} else {
			iterImageSize.width = img.cols;
		}
		
		if (img.rows % size.height > 0) {
			iterImageSize.height = (img.rows / size.height + 1) * size.height;
		} else {
			iterImageSize.height = img.rows;
		}
		
		int extentedRows = iterImageSize.height - img.rows;
		int extentedCols = iterImageSize.width - img.cols;
		int avgColor = cv::mean(img)[0] - 5;
		CImage result = CImage(iterImageSize, cv::DataType<uchar>::type, avgColor);
		CImage roi = result(cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);
		return result;
	}
}
