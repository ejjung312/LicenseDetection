#include <opencv2/opencv.hpp>

#include "LicenseDetection.h"
#include "objectbbox.h"
#include "yolo11.h"

void InitLicenseModel(const char* modelPath, const char* classNames)
{
    if (!g_license_model)
    {

        g_license_model = new Yolo11(modelPath, 0.25f, 0.25f,
            [](int lbl_id, const std::string lbl)
            { return lbl_id >= 0 && lbl_id <= 8; },
            classNames);
    }
}

void LicenseDetection(unsigned char* data, int width, int height, int stride)
{
    try
    {
        if (!g_license_model) {
            std::cerr << "Model not initialized!" << std::endl;
            return;
        }

        cv::Mat frame(height, width, CV_8UC3, data, stride);
        if (frame.empty()) {
            std::cerr << "Frame creation failed!" << std::endl;
            return;
        }

        std::vector<ObjectBBox> bbox_list = g_license_model->detect(frame);

        for (auto& bbox : bbox_list)
        {
            bbox.draw(frame, cv::Scalar(255, 255, 0));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "OpenCV Exception: " << e.what() << std::endl;
    }
}

void ReleaseLicenseModel()
{
    if (g_license_model)
    {
        delete g_license_model;
        g_license_model = nullptr;
    }
}