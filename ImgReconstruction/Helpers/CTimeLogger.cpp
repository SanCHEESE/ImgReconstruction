//
//  CTimeLogger.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 22.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CTimeLogger.h"

using namespace std;
using ms = chrono::milliseconds;
using get_time = chrono::system_clock;

std::chrono::time_point<std::chrono::system_clock> CTimeLogger::_start;

void CTimeLogger::StartLogging(const std::string& desc)
{
	_start = get_time::now();
}

void CTimeLogger::PrintTime()
{
	auto end = get_time::now();
	auto duration = std::chrono::duration_cast<ms>(end - _start).count();
	std::clog << "\ttime: " << duration << " ms" << std::endl;
}