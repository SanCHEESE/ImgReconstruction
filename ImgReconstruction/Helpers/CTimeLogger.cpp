//
//  CTimeLogger.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 22.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CTimeLogger.hpp"

clock_t CTimeLogger::_time = 0;
clock_t CTimeLogger::_totalTime = 0;

void CTimeLogger::StartLogging()
{
    _time = clock();
}

void CTimeLogger::Print(const std::string& decr)
{
    double diff = ((double)clock()-(double)_time);
    _totalTime += diff;
    std::cout << "◴◵◶◷◴◵◶◷◴◵◶◷◴◵◶◷◴◵◶◷◴◵◶◷◴◵◶◷" << std::endl;
    std::cout << decr << "\n\t" << "Time: " << diff/CLOCKS_PER_SEC << " s" << std::endl;
    std::cout << "◴◵◶◷◴◵◶◷◴◵◶◷◴◵◶◷◴◵◶◷◴◵◶◷◴◵◶◷" << std::endl;
}

void CTimeLogger::PrintTotalTime()
{
    std::cout << "Total execution time: " << (double)_totalTime/CLOCKS_PER_SEC << " s" << std::endl;
}