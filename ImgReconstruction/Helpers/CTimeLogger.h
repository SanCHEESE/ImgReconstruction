//
//  CTimeLogger.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 22.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include <time.h>

class CTimeLogger
{
public:
	static void StartLogging(const std::string& desc = "");
	static void Print(const std::string& decr);
	static void PrintTotalTime();
private:
	static clock_t _time;
	static clock_t _totalTime;
};
