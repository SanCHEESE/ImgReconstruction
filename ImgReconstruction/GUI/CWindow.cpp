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

CImage CWindow::GetImage() const
{
    CImage result;
    _image.copyTo(result);
    return result;
}

void CWindow::DrawRect(const cv::Rect rect, const cv::Scalar &color, int thickness)
{
    cv::rectangle(_image, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, thickness);
    cv::imshow(_name, _image);
}

void CWindow::DrawRect(const cv::Rect rect, TRectColor colorType, int thickness)
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
    DrawRect(rect, color, thickness);
}

void CWindow::DrawRects(const std::vector<DrawableRect>& rects)
{
    for (int i = 0; i < rects.size(); i++ ) {
        cv::rectangle(_image, cv::Point(rects[i].rect.x, rects[i].rect.y), cv::Point(rects[i].rect.x + rects[i].rect.width, rects[i].rect.y + rects[i].rect.height), rects[i].color, rects[i].thickness);
    }
    cv::imshow(_name, _image);
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
        } else if ((char)c == 's') {
            utils::SaveImage(SaveImgPath + _name + ".jpg", _image);
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
            
            if (window->_drawMode == TDrawModeStamp) {
                window->Update(window->_originalImage);
                window->_stampRect = cv::Rect(x - window->_maxBoxSideSize/2,
                                              y - window->_maxBoxSideSize/2,
                                              window->_maxBoxSideSize,
                                              window->_maxBoxSideSize);
                window->DrawRect(window->_stampRect, TRectColorBlue);
            }
            break;
        case CV_EVENT_LBUTTONDOWN:
            window->_isDrawing = true;
            if (window->_drawMode == TDrawModeStamp) {
                window->_drawingBox = window->_stampRect;
            }

            break;
        case CV_EVENT_LBUTTONUP:
            window->_isDrawing = false;
            window->_drawMode = TDrawModeNone;
            window->Update(window->_originalImage);
            window->DrawRect(window->_drawingBox, TRectColorRed);
            if (window->_drawingBox.height > 0 && window->_drawingBox.width > 0) {
                window->delegate->WindowDidSelectPatch(window->_name, window->_drawingBox);
            }
            break;
        case CV_EVENT_RBUTTONUP:
            window->Update(window->_originalImage);
            // раскомментировать, если надо выделить патч конкретного размера
            window->_drawMode = (TDrawMode)((window->_drawMode + 1) % TDrawModeNone);
            
            break;
        default:
            break;
    }
}