//
//  CImage.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImage.hpp"
#include "CImageProcessor.hpp"
#include "CBlurMeasurer.hpp"

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

#pragma mark - CImage
CImage::CPatchIterator CImage::GetPatchIterator(const cv::Size& size, const cv::Point& offset, const cv::Rect& pointingRect) const
{
    return CPatchIterator(this, size, offset, pointingRect);
}

CImage CImage::GetResizedImage(const cv::Size &size) const
{
    CImage resizedImage = utils::Resize(*this, size);
    return resizedImage;
}

CImage CImage::GetPatch(const cv::Rect &rect) const
{
    return CImage(*this, rect);
}

std::vector<CImage> CImage::GetAllPatches(const cv::Size& size, const cv::Point offset) const
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

void CImage::CopyMetadataTo(CImage &image) const
{
    image._frame = this->_frame;
}

#pragma mark - CPatchIterator

inline bool CImage::CPatchIterator::HasNext()
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
    
    // делаем матрицу размера size с подматрицей
    cv::Rect patchFrame = cv::Rect(_pointingRect.x, _pointingRect.y, maxCol - _pointingRect.x, maxRow - _pointingRect.y);
    CImage patch = (*_iterImage)(patchFrame);
    patch._frame = patchFrame;
	
    if (_pointingRect.width + _pointingRect.x < _iterImage->cols - 1) {
        // не у правого края
        _pointingRect.x += _offset.x;
	} else if (_pointingRect.width + _pointingRect.x >= _iterImage->cols - 1) {
		// сдвигаемся на след. строку
		_pointingRect.y += _offset.y;
		_pointingRect.x = 0;
	}
    
    return patch;
}