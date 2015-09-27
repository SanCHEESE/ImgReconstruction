//
//  Window.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CWindow.hpp"

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

void CWindow::DrawRect(const cv::Rect rect)
{
    cv::rectangle(_image, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), cv::Scalar(0, 0, 255), 1.5);
    cv::imshow(_name, _image);
}

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
                window->_drawingBox.width = x - window->_drawingBox.x;
                window->_drawingBox.height = y - window->_drawingBox.y;
                window->DrawRect(window->_drawingBox);
            }
            break;
        case CV_EVENT_LBUTTONDOWN:
            window->_isDrawing = true;
            window->_drawingBox = cv::Rect(x, y, 0, 0);
            break;
        case CV_EVENT_LBUTTONUP:
            window->_isDrawing = false;
            window->Update(window->_originalImage);
            window->DrawRect(window->_drawingBox);
            break;
        default:
            break;
    }
}