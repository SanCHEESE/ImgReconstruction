//
//  main.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CWindow.hpp"
#include "CThreshBinarizer.hpp"
#include "CImageProcessor.hpp"

static const std::string PathToImg = "../../images/img1.jpg";
static const std::string WindowName = "Press Esc to quit";

const int ThresholdValue = 120;

using namespace cv;

int main(int argc, char** argv)
{
    CImage image = CImage(PathToImg, IMREAD_GRAYSCALE);
    CThreshBinarizer binarizer = CThreshBinarizer(ThresholdValue);
    
    CImage colorBinImage;
    cvtColor(binarizer.Binarize(image), colorBinImage, CV_GRAY2RGBA, 4);
    
    CWindow window = CWindow(WindowName, colorBinImage);
    window.Show();

    window.StartObservingMouse();
    window.ObserveKeyboard();
    
    return 0;
}