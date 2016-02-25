//
//  main.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CWindow.hpp"
#include "CImageProcessor.hpp"

#if TEST_BLUR_METRICS
static const std::string PathToImg = "../../images/blurredImageTest.jpg";
#endif

static const std::string WindowName = "Press Esc to quit";

using namespace cv;

int main(int argc, char** argv)
{
    if (argc <= 1) {
        std::wcerr << "Too few arguments!" << std::endl;
    }
#if TEST_BLUR_METRICS
    CImage image = CImage(PathToImg, IMREAD_GRAYSCALE);
#endif 
	CImage image = CImage(argv[1], IMREAD_GRAYSCALE);
    
	if (image.empty()) {
        std::wcerr << "No such image!" << std::endl;
		return 1;
	}
	
#if ENABLE_GUI
	CWindow window = CWindow(WindowName);
	CImageProcessor imProc = CImageProcessor(window);
#else 
    CImageProcessor imProc = CImageProcessor();
#endif
    
    for () {
        
    }
    
	imProc.StartProcessingChain(image, "!result");
	
	return 0;
}