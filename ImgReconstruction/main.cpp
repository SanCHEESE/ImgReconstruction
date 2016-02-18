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
#else 
static const std::string PathToImg = "../../images/img.jpg";
#endif

static const std::string WindowName = "Press Esc to quit";

using namespace cv;

int main(int argc, char** argv)
{
	CImage image = CImage(PathToImg, IMREAD_GRAYSCALE);
	if (image.empty()) {
		return 1;
	}
	
	CWindow window = CWindow(WindowName);
	CImageProcessor imProc = CImageProcessor(window);
	imProc.StartProcessingChain(image);
	
	return 0;
}