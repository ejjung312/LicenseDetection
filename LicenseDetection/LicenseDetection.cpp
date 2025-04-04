#include <opencv2/opencv.hpp>

#include "LicenseDetection.h"
#include "objectbbox.h"
#include "yolo11.h"

void ConvertToGray(unsigned char* data, int width, int height, int stride)
{
    // 입력 데이터는 BGRA 포맷 (3채널)
    cv::Mat bgra(height, width, CV_8UC3, data, stride);

    // Grayscale로 변환
    cv::Mat gray;
    cv::cvtColor(bgra, gray, cv::COLOR_BGRA2GRAY);

    // 다시 BGRA로 변환하여 원본 메모리에 덮어쓰기
    cv::cvtColor(gray, bgra, cv::COLOR_GRAY2BGRA);
}

void LicenseDetection(const char* licenseModelPath, const char* classNames, unsigned char* data, int width, int height, int stride)
{
    try
    {
        Yolo11 license_model(licenseModelPath, 0.25f, 0.25f,
            [](int lbl_id, const std::string lbl)
            { return lbl_id >= 0 && lbl_id <= 8; },
            classNames);

        cv::Mat frame(height, width, CV_8UC3, data, stride);

        std::vector<ObjectBBox> bbox_list = license_model.detect(frame);

        for (auto& bbox : bbox_list)
        {
            /*std::cout << "Label:" << bbox.label << " Conf: " << bbox.conf;
            std::cout << "(" << bbox.x1 << ", " << bbox.y1 << ") ";
            std::cout << "(" << bbox.x2 << ", " << bbox.y2 << ")" << std::endl;*/
            bbox.draw(frame, cv::Scalar(255, 255, 0));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "OpenCV Exception: " << e.what() << std::endl;
    }
}