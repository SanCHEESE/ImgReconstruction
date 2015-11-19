//
//  utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 15.11.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "utils.hpp"

namespace utils
{
    double StandartDeviation(const CImage& img)
    {
        cv::Scalar mean, stddev;
        cv::meanStdDev(img, mean, stddev);
        return stddev[0];
    }
}
