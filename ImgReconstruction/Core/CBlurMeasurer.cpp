//
//  CBlurMeasurer.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 11.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CBlurMeasurer.hpp"
#include "CImageProcessor.hpp"

double CBlurMeasurer::Measure(const CImage& img) const
{
	switch (_measureMethod) {
		case TBlurMeasureMethodStandartDeviation:
			return MeasureUsingStdDeviation(img);
		case TBlurMeasureMethodDynamicRange:
			return MeasureUsingDynamicRange(img);
		case TBlurMeasureMethodFFT:
			return MeasureUsingFFT(img);
		default:
			break;
	}
	
	return 0;
}

double CBlurMeasurer::MeasureUsingStdDeviation(const CImage &img) const
{
	return utils::StandartDeviation(img);
}

double CBlurMeasurer::MeasureUsingDynamicRange(const CImage &img) const
{
	int histSize = 256;
	float range[] = {0, 256} ;
	const float* histRange = {range};
	
	cv::Mat histogram;
	cv::calcHist( &img, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);
	
	std::vector<unsigned char> histArray;
	histogram.col(0).copyTo(histArray);
	auto minMaxElem = std::minmax_element(histArray.begin(), histArray.end());
	
	return std::abs(*(minMaxElem.first) - *(minMaxElem.second));
}

double CBlurMeasurer::MeasureUsingFFT(const CImage &img) const
{
	CImage fft = img.GetFFTImage();
	img.CopyMetadataTo(fft);
	fft.Save();
	return utils::MeasureBlurWithFFTImage(fft, BlurMetricRadiusRatio);
}