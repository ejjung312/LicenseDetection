#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <set>
#include <algorithm>
#include <iterator>

#include <opencv2/dnn.hpp>

#include "detection_result.h"

class CentroidTracker {
public:
    // ������ ����ȯ ����
    explicit CentroidTracker(int maxDisappeared);
    void register_Object(int cX, int cY);
    std::vector<std::pair<int, std::pair<int, int>>> update(DetectionResult* boxes, int* resultCount);
    // <ID, centroids>
    std::vector<std::pair<int, std::pair<int, int>>> objects;
    //make buffer for path tracking
    std::map<int, std::vector<std::pair<int, int>>> path_keeper;
private:
    int maxDisappeared;
    int nextObjectID;
    static double calcDistance(double x1, double y1, double x2, double y2);
    // <ID, count>
    std::map<int, int> disappeared;
};

static CentroidTracker* g_centroid_tracker = nullptr;