//
//  CImage.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImage.hpp"


#pragma mark - CImage
CImage::CPatchIterator CImage::GetPatchIterator(const cv::Size& size, const cv::Point& offset, const cv::Rect& pointingRect)
{
    return CPatchIterator(this, size, offset, pointingRect);
}

CImage CImage::GetPatch(const cv::Rect &rect)
{
    return CImage(*this, rect);
}

#pragma mark - CPatchIterator

bool CImage::CPatchIterator::HasNext()
{
    return _pointingRect.width + _pointingRect.x + _offset.x < _iterImage->cols || _pointingRect.height + _pointingRect.y + _offset.y < _iterImage->rows;
}

CImage CImage::CPatchIterator::GetNext()
{
    int maxRow = MAX(_pointingRect.height + _pointingRect.y, _iterImage->rows);
    int maxCol = MAX(_pointingRect.width + _pointingRect.x, _iterImage->cols);
    
    // делаем матрицу размера size с подматрицей
    cv::Mat patch = (*_iterImage)(cv::Rect(_pointingRect.x, _pointingRect.y, maxCol - _pointingRect.x, maxRow - _pointingRect.y));
    // нормализуем патч до нужного размера
    cv::Mat normPatch = cv::Mat(_size, CV_8U, 255);
    
    // копируем матрицу
    for (int i = 0; i < patch.rows; i++) {
        for (int j = 0; j < patch.cols; j++) {
            normPatch.data[normPatch.channels()*(patch.cols * i + j)] = patch.at<uint8_t>(i, j);
        }
    }
    
    if (_pointingRect.width + _pointingRect.x < _iterImage->cols - 1) {
        // не у правого края
        _pointingRect.x += _offset.x;
    } else if (_pointingRect.height + _pointingRect.y < _iterImage->rows - 1 ) {
        // не у низа
        _pointingRect.y += _offset.y;
    }
    
    if (_pointingRect.width + _pointingRect.x >= _iterImage->cols - 1) {
        _pointingRect.y += _offset.y;
    }
    
    return CImage((*_iterImage)(_pointingRect));
}