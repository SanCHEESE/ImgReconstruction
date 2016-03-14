//
//  main.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.h"

using namespace cv;

int main(int argc, char** argv)
{
    if (argc <= 1) {
        std::wcerr << "Too few arguments!" << std::endl;
    }
	CImage image = CImage(argv[1], IMREAD_GRAYSCALE);
    
	if (image.empty()) {
        std::wcerr << "No such image!" << std::endl;
		return 1;
	}
	
    CImageProcessor imProc = CImageProcessor();
    imProc.ProcessImage(image, "result");
	
	return 0;
}