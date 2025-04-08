#pragma once

struct DetectionResult {
	int x;
	int y;
	int width;
	int height;
	float confidence;
	int classId;
};