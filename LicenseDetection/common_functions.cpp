#include "common_functions.h"
#include "objectbbox.h"

#include <opencv2/core/ocl.hpp>

using Clock = std::chrono::high_resolution_clock;

std::string GetResourcePath(const std::string& filename) {
    // MAX_PATH: Windows에서 정의된 최대 경로 길이
    // {0}으로 초기화하여 배열의 모든 요소를 \0(Null 문자)로 채움
    char path[MAX_PATH] = { 0 };
    // 현재 실행 중인 실행 파일의 전체 경로를 가져오는 Windows API 함수
    // NULL: 현재 실행중인 모듈(실행 파일)의 경로
    // path: path에 저장
    GetModuleFileNameA(NULL, path, MAX_PATH);

    // std::string dirPath = "path 값";
    std::string dirPath(path);
    // 실행 파일 경로에서 마지막 디렉터리 구분자(\\ 또는 /)의 위치 반환
    size_t pos = dirPath.find_last_of("\\/");
    // 위치값을 찾았을 경우 pos 위치까지 자른 부분 반환
    dirPath = (pos != std::string::npos) ? dirPath.substr(0, pos) : dirPath;

    return dirPath + "\\" + filename;
}

/**
 * IoU - 두 개의 영역(예측된 영역과 실제 영역)의 겹치는 정도를 나타내며, 값이 클수록 정확도가 높음
 * IoU = 겹친영역(intersection) / 전체영역(union)
 */
float calculateIoU(const DetectionResult& box1, const DetectionResult& box2, const cv::Point2f& scale) {
    float x1 = (box1.x - box1.width / 2) * scale.x;
    float y1 = (box1.y - box1.height / 2) * scale.y;
    float x2 = (box1.x + box1.width / 2) * scale.x;
    float y2 = (box1.y + box1.height / 2) * scale.y;
    cv::Rect rect1 = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));

    x1 = (box2.x - box2.width / 2) * scale.x;
    y1 = (box2.y - box2.height / 2) * scale.y;
    x2 = (box2.x + box2.width / 2) * scale.x;
    y2 = (box2.y + box2.height / 2) * scale.y;
    cv::Rect rect2 = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));

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
    std::cout << "[" << title << " 시간] " << process_time.count() << " ms" << std::endl;
}