//
//  ImageComparatorL1.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "IImageComparator.hpp"

class CImageComparatorL1 : public IImageComparator
{
public:
    virtual double Compare(const CImage& img1, const CImage& img2) const;
};
