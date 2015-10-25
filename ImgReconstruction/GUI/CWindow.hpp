//
//  Window.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//
#pragma once

typedef enum : int {
    TDrawModeStamp,
    TDrawModeDraw,
    TDrawModeNone
} TDrawMode;

typedef enum : int {
    TRectColorRed,
    TRectColorBlue,
    TRectColorGreen,
} TRectColor;

struct DrawableRect {
    cv::Rect rect;
    cv::Scalar color;
    int thickness;
};

class CWindowDelegate
{
public:
    virtual ~CWindowDelegate() {};
    virtual void WindowDidSelectPatch(const std::string& windowName, const cv::Rect& patchRect) = 0;
};


class CWindow
{
public:
    CWindow(const std::string& name) : _name(name)
    {
        _drawMode = TDrawModeStamp;
        _isDrawing = false;
    };
    CWindow(const std::string& name, CImage& image) : _image(image), _name(name)
    {
        _image.copyTo(_originalImage);
        _drawMode = TDrawModeStamp;
        _isDrawing = false;
    };
    
    void Show(int flags = CV_WINDOW_AUTOSIZE);
    void Update(const CImage& img);
    CImage GetImage() const;
    
    void SetOriginalImage(const CImage& originalImage);
    void SetMaxBoxSideSize(int boxSideSize);
    
    void ObserveKeyboard();
    void StartObservingMouse();

    void DrawRect(const cv::Rect rect, const cv::Scalar& color, int thickness = 1);
    void DrawRect(const cv::Rect rect, TRectColor colorType, int thickness = 1);
    void DrawRects(const std::vector<DrawableRect>& rects);
    
    CWindowDelegate* delegate;
private:
    static void MouseCallback(int event, int x, int y, int flags, void* param);
    
    std::string _name;
    CImage _image;
    
    CImage _originalImage;
    
    cv::Rect _stampRect;
    cv::Rect _drawingBox;
    int _maxBoxSideSize;
    TDrawMode _drawMode;
    
    bool _isDrawing;
    
    std::vector<DrawableRect> rectsToDraw;
};
