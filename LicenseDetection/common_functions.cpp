#include "common_functions.h"

#include <string>
#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>


using Clock = std::chrono::high_resolution_clock;

std::string GetResourcePath(const std::string& filename) {
    // MAX_PATH: Windows���� ���ǵ� �ִ� ��� ����
    // {0}���� �ʱ�ȭ�Ͽ� �迭�� ��� ��Ҹ� \0(Null ����)�� ä��
    char path[MAX_PATH] = { 0 };
    // ���� ���� ���� ���� ������ ��ü ��θ� �������� Windows API �Լ�
    // NULL: ���� �������� ���(���� ����)�� ���
    // path: path�� ����
    GetModuleFileNameA(NULL, path, MAX_PATH);

    // std::string dirPath = "path ��";
    std::string dirPath(path);
    // ���� ���� ��ο��� ������ ���͸� ������(\\ �Ǵ� /)�� ��ġ ��ȯ
    size_t pos = dirPath.find_last_of("\\/");
    // ��ġ���� ã���� ��� pos ��ġ���� �ڸ� �κ� ��ȯ
    dirPath = (pos != std::string::npos) ? dirPath.substr(0, pos) : dirPath;

    return dirPath + "\\" + filename;
}

void getRectPosition(DetectionResult& box, float scale_x, float scale_y)
{
    float x1 = (box.cx - box.width / 2) * scale_x;
    float y1 = (box.cy - box.height / 2) * scale_y;
    float x2 = (box.cx + box.width / 2) * scale_x;
    float y2 = (box.cy + box.height / 2) * scale_y;

    box.x1 = x1;
    box.y1 = y1;
    box.x2 = x2;
    box.y2 = y2;
}

/**
 * IoU - �� ���� ����(������ ������ ���� ����)�� ��ġ�� ������ ��Ÿ����, ���� Ŭ���� ��Ȯ���� ����
 * IoU = ��ģ����(intersection) / ��ü����(union)
 */
float calculateIoU(const DetectionResult& box1, const DetectionResult& box2, const cv::Point2f& scale) {
    cv::Rect rect1 = cv::Rect(cv::Point(box1.x1, box1.y1), cv::Point(box1.x2, box1.y2));
    cv::Rect rect2 = cv::Rect(cv::Point(box2.x1, box2.y1), cv::Point(box2.x2, box2.y2));

    auto intersection = rect1 & rect2;
    if (intersection.empty()) return 0.0f;

    float intersection_area = intersection.area();
    float union_area = rect1.area() + rect2.area() - intersection_area;

    return intersection_area / union_area;
}

///////////////////////////////////////////////////////////////////

void selectDNNBackendAndTarget(cv::dnn::Net& net_) {
#if defined(CUDA_ACC)
    net_.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    net_.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    std::cout << "Using CUDA" << std::endl;
#elif defined(OPENCL_ACC)
    net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net_.setPreferableTarget(cv::dnn::DNN_TARGET_OPENCL);
    std::cout << "Using OPENCL" << std::endl;
#else
    net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    std::cout << "Using CPU" << std::endl;
#endif
}

void checkRunTime(std::string& title)
{
    auto time_start = Clock::now();

    auto time_end = Clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    std::cout << "[" << title << " �ð�] " << process_time.count() << " ms" << std::endl;
}