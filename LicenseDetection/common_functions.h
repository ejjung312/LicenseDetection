#pragma once

#include <windows.h>
#include <string>

#include <opencv2/opencv.hpp>

#include "detection_result.h"

std::string GetResourcePath(const std::string& filename);

void getRectPosition(DetectionResult& box, float scale_x, float scale_y);

float calculateIoU(const DetectionResult& box1, const DetectionResult& box2, const cv::Point2f& scale);

///////////////////////////////////////////////////////////////////

void selectDNNBackendAndTarget(cv::dnn::Net& net);

void checkRunTime(std::string& title);