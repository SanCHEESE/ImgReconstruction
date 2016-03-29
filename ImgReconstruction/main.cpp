//
//  main.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "common.h"

#include "CImage.h"
#include "CImageSubprocessorHolder.h"
#include "CImageProcessor.h"
#include "CBlurMetricsTester.h"

#include "CTimeLogger.h"

char* Help = "USAGE: imgReconstruction [in_path] [out_path] (opt)[config_path]\n \
					in_path - path to input image\n \
					out_path - path to output image\n \
					config_path - path to json formatted config(optional)\n";

using namespace cv;

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::wcerr << "Too few arguments! \n" << Help << std::endl;
		return 1;
	}

#ifdef _DEBUG
	for (int i = 0; i < argc; i++) {
		std::cout << argv[i] << std::endl;
	}
#endif


	if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
		std::wcerr << Help << std::endl;
		return 2;
	} else if (std::string(argv[1]) == "-t" || std::string(argv[1]) == "--test") {
		if (std::string(argv[2]) == "blur") {
			CImage image = CImage(argv[3], IMREAD_GRAYSCALE);

			if (image.empty()) {
				std::wcerr << "No such image!" << std::endl;
				return 1;
			}

			CBlurMetricsTester tester(image);
			tester.Test();

			system("pause");

		} else {
			std::wcerr << "Unknown option" << std::endl;
		}
	} else {
		CImage image = CImage(argv[1], IMREAD_GRAYSCALE);

		if (image.empty()) {
			std::wcerr << "No such image!" << std::endl;
			return 1;
		}

		CImageSubprocessorHolder::GetInstance().Configure(argv[3]);

		CImageProcessor imProc = CImageProcessor();
		imProc.ProcessImage(image, argv[2]);

		CTimeLogger::PrintTotalTime();
	}
	
	return 0;
}