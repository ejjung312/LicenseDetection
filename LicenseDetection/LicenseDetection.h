#pragma once

#ifdef LICENSE_DETECTION_EXPORTS
#define LICENSE_DETECTION_API __declspec(dllexport)
#else
#define LICENSE_DETECTION_API __declspec(dllimport)
#endif

extern "C" {
	LICENSE_DETECTION_API void ConvertToGray(unsigned char* data, int width, int height, int stride);

	LICENSE_DETECTION_API void LicenseDetection(const char* modelPath, unsigned char* data, int width, int height, int stride);
}