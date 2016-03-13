//
//  CParameter.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 25.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CParameter.h"

std::ostream& operator<<(std::ostream& os, const CParameter& param)
{
    if (param._hasDoubleValue) {
        os << param._doubleValue;
    }
    
    if (param._hasSizeValue) {
        os << param._sizeValue;
    }
    
    if (param._hasPointValue) {
        os << param._pointValue;
    }
    
    if (param._hasIntValue) {
        os << param._intValue;
    }
    
    return os;
}
