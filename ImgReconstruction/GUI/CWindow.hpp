//
//  Window.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//
#pragma once

typedef enum : int {
    TRectColorRed,
    TRectColorBlue,
    TRectColorGreen,
} TRectColor;

class CWindowDelegate
{
public:
    virtual ~CWindowDelegate() {};
    virtual void WindowDidSelectPatch(const CImage& img, const cv::Rect& patchRect) = 0;
};


class CWindow
{
public:
    CWindow(const std::string& name) : _name(name)
    {
        _isDrawing = false;
    };
    CWindow(const std::string& name, CImage& image) : _image(image), _name(name)
    {
        _image.copyTo(_originalImage);
        _isDrawing = false;
    };
    
    void Show(int flags = CV_WINDOW_AUTOSIZE);
    void Update(const CImage& img);
    
    void SetOriginalImage(const CImage& originalImage);
    void SetMaxBoxSideSize(int boxSideSize);
    
    void ObserveKeyboard();
    void StartObservingMouse();

    void DrawRect(const cv::Rect rect, const cv::Scalar& color);
    void DrawRect(const cv::Rect rect, TRectColor colorType);
    
    CWindowDelegate* delegate;
private:
    static void MouseCallback(int event, int x, int y, int flags, void* param);
    
    std::string _name;
    CImage _image;
    
    CImage _originalImage;
    
    cv::Rect _drawingBox;
    int _maxBoxSideSize;
    bool _isDrawing;
};
