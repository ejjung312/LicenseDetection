#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>

class PaddleOCR {
private:
	cv::dnn::Net net_;

public:
	PaddleOCR(const std::string& model_path, float min_conf = 0.45f);
	cv::Mat ocr(const cv::Mat& image);
};

static PaddleOCR* g_ocr_model = nullptr;