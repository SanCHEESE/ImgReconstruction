//
//  CTimeLogger.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 22.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "common.h"

#include <chrono>

class CTimeLogger
{
public:
	static void StartLogging(const std::string& desc = "");
	static void PrintTime(const std::string& desc = "");
private:
	static std::chrono::time_point<std::chrono::system_clock> _start;
};
