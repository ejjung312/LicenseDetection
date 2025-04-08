#pragma once

struct DetectionResult {
	int x1;
	int y1;
	int x2;
	int y2;
	int width;
	int height;
	float confidence;
	int classId;
};