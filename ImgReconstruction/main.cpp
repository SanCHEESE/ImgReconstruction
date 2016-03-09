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
//    imProc.StartProcessingChain(image, "result");

    
//    for (int i = 1; i < 100; i++) {
//        imProc.SetIterCount(i);
//    imProc.StartProcessingChain(image, "result_" + std::to_string(i));
    //    }
    
    int counter = 0;
    
    for (int metric = TImageCompareMetricL1; metric < TImageCompareMetricNone; metric++) {
        imProc.GetConfig().SetParam(CompMetricConfigKey, metric);
        for (int compSum = TCompSumStd; compSum < TCompSumNone; compSum++) {
            imProc.GetConfig().SetParam(CompSumConfigKey, compSum);
            double startEps = metric == TImageCompareMetricL1 ? DefaultComparisonEpsL1 : (compSum == TCompSumBorder ? DefaultComparisonEpsBorderL2 : DefaultComparisonEpsStdL2 );
            double endEps = metric == TImageCompareMetricL1 ? 1000 : (compSum == TCompSumBorder ? 1100 : 8000 );
            double epsInc = metric == TImageCompareMetricL1 ? 100 : (compSum == TCompSumBorder ? 100 : 500 );
            for (double eps = startEps; eps < endEps; eps += epsInc) {
                imProc.GetConfig().SetParam(metric == TImageCompareMetricL1 ? ComparisonEpsL1ConfigKey : ComparisonEpsL2ConfigKey, eps);
                for (int accSumMethod = 0; accSumMethod < TAccImageSumMethodNone; accSumMethod++) {
                    imProc.GetConfig().SetParam(AccImageSumMethodConfigKey, accSumMethod);
                    imProc.StartProcessingChain(image, "result_" + std::to_string(counter));
                    imProc.GetConfig().WriteToFile(SaveImgPath + "result_" + std::to_string(counter) + "_params.txt");
                    counter++;
                }
            }
        }
    }
	
	return 0;
}