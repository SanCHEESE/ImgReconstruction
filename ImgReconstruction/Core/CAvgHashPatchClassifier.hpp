//
//  CAvgHashPatchClassifier.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IPatchClassifier.h"
#include "CTimeLogger.h"
#include "CImagePatch.h"

class CAvgHashPatchClassifier : public IPatchClassifier
{
public:
	virtual std::map<uint64, std::vector<CImagePatch>> Classify(std::vector<CImagePatch>& patches) const
	{
		CTimeLogger::StartLogging();

		std::map<uint64, std::vector<CImagePatch>> classes;

		for (CImagePatch& patch : patches) {
			auto aClass = classes.find(patch.AvgHash());
			if (aClass == classes.end()) {
				classes[patch.AvgHash()] = std::vector<CImagePatch>(1, patch);
			} else {
				(*aClass).second.push_back(patch);
			}
		}

		CTimeLogger::Print("Patch classification: ");

		return classes;
	}
};