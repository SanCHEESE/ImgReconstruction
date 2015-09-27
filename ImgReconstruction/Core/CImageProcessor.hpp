//
//  CImageProcessor.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

class CImageProcessor
{
public:
    static CImage GetPatchImageFromImage(const CImage& img, const cv::Rect& patchRect);
};