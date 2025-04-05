#pragma once

extern "C" {
	__declspec(dllexport) void LicenseDetection(const char* modelPath, const char* classNames, unsigned char* data, int width, int height, int stride);
}