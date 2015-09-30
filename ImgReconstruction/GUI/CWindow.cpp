//
//  Window.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CWindow.hpp"

static const cv::Scalar RedColor = cv::Scalar(0, 0, 255);
static const cv::Scalar GreenColor = cv::Scalar(0, 255, 0);
static const cv::Scalar BlueColor = cv::Scalar(255, 0, 0);

#pragma mark - Display stuff

void CWindow::Show(int flags)
{
    cv::namedWindow(_name, flags);
    if (_image.rows != 0 && _image.cols != 0) {
        cv::imshow(_name, _image);
    }
}

void CWindow::Update(const CImage& img)
{
    cv::imshow(_name, img);
    img.copyTo(_image);
}


void CWindow::DrawRect(const cv::Rect rect, const cv::Scalar &color)
{
    cv::rectangle(_image, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 1.5);
    cv::imshow(_name, _image);
}

void CWindow::DrawRect(const cv::Rect rect, TRectColor colorType)
{
    cv::Scalar color;
    switch (colorType) {
        case TRectColorRed:
            color = RGB(255, 0, 0);
            break;
        case TRectColorBlue:
            color = RGB(0, 0, 255);
            break;
        case TRectColorGreen:
            color = RGB(0, 255, 0);
            break;
        default:
            assert(false);
            break;
    }
    DrawRect(rect, color);
}

void CWindow::SetOriginalImage(const CImage &originalImage)
{
    originalImage.copyTo(_originalImage);
}

void CWindow::SetMaxBoxSideSize(int boxSideSize)
{
    _maxBoxSideSize = boxSideSize;
}

#pragma mark - Events observing

void CWindow::ObserveKeyboard()
{
    while (true) {
        int c = cv::waitKey(20);
        if (c == 27) {
            break;
        }
    }
}

void CWindow::StartObservingMouse()
{
    cv::setMouseCallback(_name, &CWindow::MouseCallback, this);
}

void CWindow::MouseCallback(int event, int x, int y, int flags, void *param)
{
    CWindow *window = (CWindow *)param;
    
    switch (event) {
        case CV_EVENT_MOUSEMOVE:
            if (window->_isDrawing) {
                window->Update(window->_originalImage);
                window->_drawingBox.width = MIN(window->_maxBoxSideSize, MAX(x - window->_drawingBox.x, y - window->_drawingBox.y));
                window->_drawingBox.height = window->_drawingBox.width;
                window->DrawRect(window->_drawingBox, TRectColorRed);
            }
            break;
        case CV_EVENT_LBUTTONDOWN:
            window->_isDrawing = true;
            window->_drawingBox = cv::Rect(x, y, 0, 0);
            break;
        case CV_EVENT_LBUTTONUP:
            window->_isDrawing = false;
            window->Update(window->_originalImage);
            window->DrawRect(window->_drawingBox, TRectColorRed);
            if (window->_drawingBox.height > 0 && window->_drawingBox.width > 0) {
                window->delegate->WindowDidSelectPatch(window->_originalImage, window->_drawingBox);
            }
            break;
        default:
            break;
    }
}