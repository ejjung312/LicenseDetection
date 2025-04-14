#pragma once

#include "detection_result.h"

extern "C" {
	__declspec(dllexport) void InitLicenseModel(const char* modelPath, const char* classNames);

	__declspec(dllexport) void LicenseDetection(unsigned char* data, int width, int height, int stride, DetectionResult* results, int* resultCount);

	__declspec(dllexport) void ReleaseLicenseModel();
}