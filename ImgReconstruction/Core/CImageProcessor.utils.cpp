//
//  CImageProcessor.utils.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 13.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CImageProcessor.h"
#include "CTimeLogger.h"

std::map<int, std::vector<CImagePatch>> CImageProcessor::Clusterize(const std::vector<CImagePatch>& aClass)
{
	std::map<int, std::vector<CImagePatch>> clusters;
	auto aClassCopy = std::vector<CImagePatch>(aClass);

	IImageComparator* comparator = _subprocHolder->ImageComparator();
	int aClassIdx = 0;
	for (int i = 0; i < aClassCopy.size(); i++) {

		std::vector<CImagePatch> similarPatches;
		similarPatches.push_back(aClassCopy[i]);
		aClassCopy[i].aClass = aClassIdx;

		for (int j = 1; j < aClassCopy.size(); j++) {
			if (comparator->Equal(aClassCopy[i], aClassCopy[j])) {
				similarPatches.push_back(aClassCopy[j]);
				aClassCopy[j].aClass = aClassIdx;
				auto it = aClassCopy.begin() + j;
				aClassCopy.erase(it);
				j--;
			}
		}

		aClassCopy.erase(aClassCopy.begin());
		clusters[aClassIdx] = similarPatches;

		aClassIdx++;
		i--;
	}

	return clusters;
}