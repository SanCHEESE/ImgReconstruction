//
//  Window.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//
#pragma once

class CWindow
{
public:
    CWindow(const std::string& name) : _name(name) {};
    CWindow(const std::string& name, CImage& image) : _image(image), _name(name)
    {
        _image.copyTo(_originalImage);
    };
    
    void Show(int flags = CV_WINDOW_AUTOSIZE);
    void Update(const CImage& img);

    
    void ObserveKeyboard();
    void StartObservingMouse();
public:
    void DrawRect(const cv::Rect rect);
    
    static void MouseCallback(int event, int x, int y, int flags, void* param);
    
    std::string _name;
    CImage _image;
    
    CImage _originalImage;
    
    cv::Rect _drawingBox;
    bool _isDrawing;
};
