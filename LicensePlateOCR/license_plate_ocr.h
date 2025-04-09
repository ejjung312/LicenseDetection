#pragma once

extern "C" {
	__declspec(dllexport) void InitOcrModel(const char* modelPath);

	__declspec(dllexport) void RunOcr(unsigned char* data, char* ocrText);

	__declspec(dllexport) void ReleaseOcrModel();
}