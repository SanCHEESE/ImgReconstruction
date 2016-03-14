//
//  CImageSubprocessorHolder.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CImageSubprocessorHolder.h"

#include "CPatchFilter.hpp"
#include "CPatchFetcher.hpp"
#include "CPHashPatchClassifier.hpp"
#include "CAvgHashPatchClassifier.hpp"

#include "CStdDeviationBlurMeasurer.hpp"
#include "CDynamicRangeBlurMeasurer.hpp"
#include "CFFTBlurMeasurer.hpp"
#include "CFDBlurMeasurer.hpp"

#include "CMeanBrightnessEqualizer.hpp"
#include "CDynRangeBrightnessEqualizer.hpp"

#include "CStdImageSummator.hpp"
#include "CBorderImageSummator.hpp"

#include "CL1ImageComparator.hpp"
#include "CL2ImageComparator.hpp"

#include "CAdaptiveGaussianBinarizer.hpp"
#include "CNICKBinarizer.hpp"
#include "CNiBlackBinarizer.hpp"

#include "CImageExtender.hpp"

CImageSubprocessorHolder::CImageSubprocessorHolder()
{
    
};

