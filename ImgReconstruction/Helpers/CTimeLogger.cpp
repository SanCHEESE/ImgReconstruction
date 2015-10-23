//
//  CTimeLogger.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 22.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CTimeLogger.hpp"

clock_t CTimeLogger::_time = 0;

void CTimeLogger::StartLogging()
{
    _time = clock();
}

void CTimeLogger::Print(const std::string& decr)
{
    double diff = ((double)clock()-(double)_time);
    std::cout << decr << "\n\t" << "Time: " << diff/CLOCKS_PER_SEC << " s" << std::endl;
}