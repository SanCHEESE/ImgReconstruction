//
//  IImageComparator.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

class IImageComparator
{
public:
    virtual double Compare(const CImage& img1, const CImage& img2) = 0;
};
