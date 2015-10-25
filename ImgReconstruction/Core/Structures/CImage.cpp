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

#pragma mark - CImage
CImage::CPatchIterator CImage::GetPatchIterator(const cv::Size& size, const cv::Point& offset, const cv::Rect& pointingRect) const
{
    return CPatchIterator(this, size, offset, pointingRect);
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

bool CImage::CPatchIterator::HasNext()
{
    return _pointingRect.width + _pointingRect.x + _offset.x < _iterImage->cols || _pointingRect.height + _pointingRect.y + _offset.y < _iterImage->rows;
}

CImage CImage::CPatchIterator::GetNext()
{
    int maxRow = MIN(_pointingRect.height + _pointingRect.y, _iterImage->rows);
    int maxCol = MIN(_pointingRect.width + _pointingRect.x, _iterImage->cols);
    
    // делаем матрицу размера size с подматрицей
    cv::Rect patchFrame = cv::Rect(_pointingRect.x, _pointingRect.y, maxCol - _pointingRect.x, maxRow - _pointingRect.y);
    CImage patch = (*_iterImage)(patchFrame);
    
    // нормализуем патч до нужного размера
    CImage normPatch = CImage(_size, CV_8U, 255);
    normPatch._frame = patchFrame;
    
    // копируем матрицу
    for (int i = 0; i < patch.rows; i++) {
        for (int j = 0; j < patch.cols; j++) {
            normPatch.data[normPatch.channels()*(patch.cols * i + j)] = patch.at<char>(i, j);
        }
    }
    
    if (_pointingRect.width + _pointingRect.x < _iterImage->cols - 1) {
        // не у правого края
        _pointingRect.x += _offset.x;
    } else if (_pointingRect.height + _pointingRect.y < _iterImage->rows - 1 ) {
        // не у низа
        _pointingRect.x = 0;
    }
    
    if (_pointingRect.width + _pointingRect.x >= _iterImage->cols - 1) {
        // сдвигаемся на след. строку
        _pointingRect.y += _offset.y;
    }
    
    return normPatch;
}