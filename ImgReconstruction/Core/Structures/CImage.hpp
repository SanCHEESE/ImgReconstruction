//
//  CImage.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

class CImage : public cv::Mat
{
public:
    CImage() : cv::Mat() {_frame = cv::Rect(0, 0, 0, 0);};
    CImage(const cv::Mat mat) : cv::Mat(mat) {_frame = cv::Rect(0, 0, mat.cols, mat.rows);};
    CImage(const std::string& path, int flags) : cv::Mat(cv::imread(path, flags)) {_frame = cv::Rect(0, 0, cv::Mat::cols, cv::Mat::rows);};
    CImage(int rows, int cols, int type, const cv::Scalar& scalar) : cv::Mat(rows, cols, type, scalar) {_frame = cv::Rect(0, 0, cols, rows);};
    CImage(const CImage& image, const cv::Rect& roi) : cv::Mat(image, roi) {_frame = roi;}
    CImage(const cv::Size size, int type, int value) : cv::Mat(size, type, value) {_frame = cv::Rect(0, 0, size.width, size.height);}
    
    void copyTo(CImage &image) const;
    void CopyMetadataTo(CImage& image) const;
    
    CImage GetPatch(const cv::Rect& rect) const;
    std::vector<CImage> GetAllPatches(const cv::Size& size, const cv::Point offset) const;
    cv::Rect GetFrame() const;
    
    class CPatchIterator
    {
    public:
        CPatchIterator(const CImage* const iterImage, const cv::Size& size, const cv::Point offset, const cv::Rect& pointingRect = cv::Rect()):
        _size(size), _pointingRect(pointingRect), _offset(offset), _iterImage(iterImage)
        {
            if (_pointingRect == cv::Rect(0, 0, 0, 0)) {
                _pointingRect = cv::Rect(0, 0, _size.width, _size.height);
            }
        }
        
        bool HasNext();
        CImage GetNext();
    private:
        cv::Size _size;
        cv::Rect _pointingRect;
        cv::Point _offset;
        const CImage* const _iterImage;
    };
    CPatchIterator GetPatchIterator(const cv::Size& size, const cv::Point& offset, const cv::Rect& pointingRect = cv::Rect()) const;
    
    friend std::ostream& operator<<(std::ostream& os, const CImage& img);
private:
    cv::Rect _frame;
};