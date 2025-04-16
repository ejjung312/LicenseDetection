#pragma once

#include "detection_result.h"

#ifdef _WIN32
	#ifdef LICENSEDETECTION_EXPORTS
		#define LICENSE_API __declspec(dllexport)
	#else
		#define LICENSE_API __declspec(dllimport)
	#endif
#else
	#define LICENSE_API
#endif

extern "C" {
	LICENSE_API void InitLicenseModel(const char* modelPath, const char* classNames);

	LICENSE_API void LicenseDetection(unsigned char* data, int width, int height, int stride, DetectionResult* results, int* resultCount);

	LICENSE_API void ReleaseLicenseModel();
}