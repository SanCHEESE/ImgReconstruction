//
//  main.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.hpp"

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

//    int counter = 0;
//    
//    for (int metric = TImageCompareMetricL1; metric < TImageCompareMetricNone; metric++) {
//        imProc.GetConfig().SetParam(CompMetricConfigKey, metric);
//        for (int compSum = TCompSumStd; compSum < TCompSumNone; compSum++) {
//            imProc.GetConfig().SetParam(CompSumConfigKey, compSum);
//            double startEps = metric == TImageCompareMetricL1 ? DefaultComparisonEpsL1 : (compSum == TCompSumBorder ? DefaultComparisonEpsBorderL2 : DefaultComparisonEpsStdL2 );
//            double endEps = metric == TImageCompareMetricL1 ? 1000 : (compSum == TCompSumBorder ? 1100 : 8000 );
//            double epsInc = metric == TImageCompareMetricL1 ? 100 : (compSum == TCompSumBorder ? 100 : 500 );
//            for (double eps = startEps; eps < endEps; eps += epsInc) {
//                imProc.GetConfig().SetParam(metric == TImageCompareMetricL1 ? ComparisonEpsL1ConfigKey : ComparisonEpsL2ConfigKey, eps);
//                for (int accSumMethod = 0; accSumMethod < TAccImageSumMethodNone; accSumMethod++) {
//                    imProc.GetConfig().SetParam(AccImageSumMethodConfigKey, accSumMethod);
//                    imProc.ProcessImage(image, "result_" + std::to_string(counter));
//                    imProc.GetConfig().WriteToFile(SaveImgPath + "result_" + std::to_string(counter) + "_params.txt");
//                    counter++;
//                }
//            }
//        }
//    }
	
	return 0;
}