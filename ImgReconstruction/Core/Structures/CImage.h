//
//  CImage.h
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 27.09.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

#include "common.h"
#include "IPatchIterator.h"

class IBlurMeasurer;

class CImage : public cv::Mat
{
public:
	// constructors
	CImage() : cv::Mat() { _frame = cv::Rect(0, 0, 0, 0); };
	CImage(const cv::Mat mat) : cv::Mat(mat) { _frame = cv::Rect(0, 0, mat.cols, mat.rows); };
	CImage(const std::string& path, int flags) : cv::Mat(cv::imread(path, flags)) { _frame = cv::Rect(0, 0, cv::Mat::cols, cv::Mat::rows); };
	CImage(int rows, int cols, int type, const cv::Scalar& scalar = cv::Scalar(0)) : cv::Mat(rows, cols, type, scalar) { _frame = cv::Rect(0, 0, cols, rows); };
	CImage(const CImage& image, const cv::Rect& roi) : cv::Mat(image, roi) { _frame = roi; }
	CImage(const cv::Size size, int type, int value) : cv::Mat(size, type, value) { _frame = cv::Rect(0, 0, size.width, size.height); }
	CImage(const CImage& image) { image.copyTo(*this); }

	CImage& operator=(const cv::Mat& image) { image.copyTo(*this); return *this; }

	// overrides
	void copyTo(CImage &image) const;
	void copyTo(cv::Mat &image) const;

	// copy
	void CopyMetadataTo(CImage& image) const;

	// save
	void Save(const std::string& path = "", int quality = 100, const std::string& ext = "png") const;

	// get calculated images
	CImage GetFFTImage() const;
	CImage GetResizedImage(const cv::Size& size) const;
	CImage GetSDImage(const cv::Size& filterSize) const;
	CImage GetPatch(const cv::Rect& rect) const;
	CImage GetRotatedImage(double angle) const;
	static CImage GetImageWithText(const std::string& text, const cv::Point& origin, const cv::Scalar& textColor, const cv::Scalar& bgColor, const cv::Size& imgSize);

	cv::Rect GetFrame() const;
	//template<typename T>
	void SetFrame(const cv::Rect2f& frame) { _frame = frame; }
	cv::Size GetSize() const;

	template<typename T = int>
	std::vector<CImage> GetAllPatches(const cv::Size& size, const cv::Point_<T>& offset) const
	{
		std::vector<CImage> patches;
		IPatchIterator* patchIterator = 0;
		if (typeid(T) == typeid(int)) {
			patchIterator = GetIntPatchIterator(size, offset);
		} else if (typeid(T) == typeid(float)) {
			patchIterator = GetFloatPatchIterator(size, offset);
		}
		while (patchIterator->HasNext()) {
			patches.push_back(patchIterator->GetNext());
		}
		return patches;
	}

	IPatchIterator* GetIntPatchIterator(const cv::Size& size, const cv::Point_<int>& offset, const cv::Rect_<int>& startRect = cv::Rect_<int>()) const;
	IPatchIterator* GetFloatPatchIterator(const cv::Size& size, const cv::Point_<float>& offset, const cv::Rect_<float>& startRect = cv::Rect_<float>()) const;

	friend std::ostream& operator<<(std::ostream& os, const CImage& img);
private:
	cv::Rect2f _frame;
};