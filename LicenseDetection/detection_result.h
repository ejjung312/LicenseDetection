#pragma once

struct DetectionResult {
	float cx;
	float cy;
	int x1;
	int y1;
	int x2;
	int y2;
	int width;
	int height;
	float confidence;
	int classId;
};