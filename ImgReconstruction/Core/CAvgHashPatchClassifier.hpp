//
//  CAvgHashPatchClassifier.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 14.03.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "IPatchClassifier.h"
#include "CImagePatch.h"

class CAvgHashPatchClassifier : public IPatchClassifier
{
public:
	virtual std::map<uint64, std::unordered_set<CImagePatch, CImagePatch::hasher>> Classify(std::vector<CImagePatch>& patches) const
	{
		std::map<uint64, std::unordered_set<CImagePatch, CImagePatch::hasher>> classes;

		for (CImagePatch& patch : patches) {
			auto aClass = classes.find(patch.AvgHash());
			if (aClass == classes.end()) {
				classes[patch.AvgHash()] = std::unordered_set<CImagePatch, CImagePatch::hasher>({patch});
			} else {
				(*aClass).second.insert(patch);
			}
		}

		return classes;
	}
};