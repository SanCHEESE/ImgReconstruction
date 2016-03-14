//
//  CImage.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImage.h"
#include "IBlurMeasurer.h"

#pragma mark - Overrides

void CImage::copyTo(CImage &image) const
{
	((cv::Mat)*this).copyTo(image);
	CopyMetadataTo(image);
}

void CImage::copyTo(cv::Mat &image) const
{
	((cv::Mat)*this).copyTo(image);
}

std::ostream& operator<<(std::ostream& os, const CImage& img)
{
	os << "\n";
	for (int i = 0; i < img.rows; i++) {
		os << "\t\t\t";
		for (int j = 0; j < img.cols; j++) {
			os << std::setw(4);
			os << (int)img.at<uchar>(i, j) << " ";
		}
		if (i == img.rows - 1) {
			os << "\n";
		} else {
			os << "\n\n";
		}
		
	}
	return os;
}

#pragma mark - Save

void CImage::Save(const std::string& name, int quality, const std::string& ext) const
{
    std::stringstream nameBuffer;
    if (name == "") {
        auto start = std::chrono::high_resolution_clock::now();
        auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch());
        nameBuffer << SaveImgPath + "tmp-" << nsec.count() << "." << ext;
    } else {
        nameBuffer << SaveImgPath + name + "." + ext;
    }
    
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(quality);
    cv::imwrite(nameBuffer.str(), *this, compression_params);
}

#pragma mark - Get calculated images

CImage CImage::GetSDImage(const cv::Size& filterSize) const
{
	CImage image32f;
	this->convertTo(image32f, CV_32F);
	
	CImage mu;
	blur(image32f, mu, filterSize);
	
	CImage mu2;
	blur(image32f.mul(image32f), mu2, filterSize);
	
	CImage sigma;
	cv::sqrt(mu2 - mu.mul(mu), sigma);
	
	return CImage(sigma / 255.f);
}

CImage CImage::GetFFTImage() const
{
	//expand input image to optimal size
	cv::Mat padded;
	int m = cv::getOptimalDFTSize( this->rows );
	// on the border add zero values
	int n = cv::getOptimalDFTSize( this->cols );
	cv::copyMakeBorder(*this, padded, 0, m - this->rows, 0, n - this->cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	
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
	
	return magI;
}

CImage CImage::GetResizedImage(const cv::Size &size) const
{
	CImage result;
	cv::resize(*this, result, size, cv::INTER_NEAREST);
    result._frame = cv::Rect(0, 0, size.width, size.height);
	return result;
}

#pragma mark - CImage
CImage::CPatchIterator CImage::GetPatchIterator(const cv::Size& size, const cv::Point& offset, const cv::Rect& pointingRect) const
{
	return CPatchIterator(this, size, offset, pointingRect);
}

CImage CImage::GetImageWithText(const std::string& text, const cv::Point& origin, const cv::Scalar& textColor, const cv::Scalar& bgColor, const cv::Size& imgSize)
{
    CImage textImg(imgSize.height, imgSize.width, CV_8UC1, bgColor);
    putText(textImg, text, origin, cv::FONT_HERSHEY_SIMPLEX, 0.4, textColor, 1);
    textImg._frame = cv::Rect(0, 0, imgSize.width, imgSize.height);
    
    return textImg;
}

CImage CImage::GetRotatedImage(double angle) const
{
    int len = std::max(this->cols, this->rows);
    cv::Point2f pt(len/2., len/2.);
    cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);
    
    CImage rotated;
    cv::warpAffine(*this, rotated, r, cv::Size(len, len));
    
    // cut the image
    if (_frame.width > _frame.height) {
        rotated._frame = cv::Rect(_frame.width - _frame.height + 1, 0, _frame.height - 1, _frame.width);
    } else {
        rotated._frame = cv::Rect(_frame.height - _frame.width + 1, 0, _frame.height - 1, _frame.width);
    }
    rotated = rotated(rotated._frame);
    
    return rotated;
}

CImage CImage::GetPatch(const cv::Rect &rect) const
{
	return CImage(*this, rect);
}

std::vector<CImage> CImage::GetAllPatches(const cv::Size& size, const cv::Point& offset) const
{
	std::vector<CImage> patches;
	CImage::CPatchIterator patchIterator = GetPatchIterator(size, offset);
	while (patchIterator.HasNext()) {
		patches.push_back(patchIterator.GetNext());
	}
	return patches;
}

cv::Rect CImage::GetFrame() const
{
	return _frame;
}

cv::Size CImage::GetSize() const
{
    return _frame.size();
}

void CImage::CopyMetadataTo(CImage &image) const
{
	image._frame = this->_frame;
}

#pragma mark - CPatchIterator

bool CImage::CPatchIterator::HasNext()
{
	if (_pointingRect.width + _pointingRect.x == _iterImage->cols &&
		_pointingRect.height + _pointingRect.y == _iterImage->rows) {
		return true;
	}
	
	return _pointingRect.width + _pointingRect.x + (_offset.x <= 1 ?: (_offset.x - 1)) < _iterImage->cols ||
	_pointingRect.height + _pointingRect.y + (_offset.x <= 1 ?: (_offset.x - 1)) < _iterImage->rows;
}

inline CImage CImage::CPatchIterator::GetNext()
{
	int maxRow = MIN(_pointingRect.height + _pointingRect.y, _iterImage->rows);
	int maxCol = MIN(_pointingRect.width + _pointingRect.x, _iterImage->cols);
	
	// fetching patch
	cv::Rect patchFrame = cv::Rect(_pointingRect.x, _pointingRect.y, maxCol - _pointingRect.x, maxRow - _pointingRect.y);
	CImage patch = (*_iterImage)(patchFrame);
	patch._frame = patchFrame;
	
	if (_pointingRect.width + _pointingRect.x < _iterImage->cols - 1) {
		// not near the right border
		_pointingRect.x += _offset.x;
	} else if (_pointingRect.width + _pointingRect.x >= _iterImage->cols - 1) {
		// moving to the next row
		_pointingRect.y += _offset.y;
		_pointingRect.x = 0;
	}
	
	return patch;
}