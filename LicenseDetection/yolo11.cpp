#include "yolo11.h"
#include "common_functions.h"
#include "detection_result.h"

#include <fstream>

void Yolo11::loadClassNames(const std::string& names_file) {
    std::ifstream file(names_file);
    std::cout << "file name: " << names_file << std::endl;
    assert(file.is_open() && ("Failed to open class names file " + names_file).c_str());

    std::string line;
    int class_id = 0;

    while (std::getline(file, line)) {
        // trim
        /*
        ' ' (�����̽�)
        '\t' (��)
        '\n' (����)
        '\r' (ĳ���� ����)
        '\f' (�� �ǵ�)
        '\v' (���� ��)
        */
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v")); // ������ �ƴ� ù��° ���ڿ� ��ġ�� ã�� ã�� �ε����� �߶󳻾� ���� ����
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1); // ������ �ƴ� ������ ������ ��ġ�� ã�� ������ ������ ���� ��ġ(+1)���� ���ڿ� ������ ����

        if (!line.empty()) {
            class_names_[class_id++] = line;
        }
    }

    assert(!class_names_.empty() && "No class names loaded from file `coco.names`");
}

Yolo11::Yolo11(const std::string& model_path, float min_conf, float iou_thresh, 
                ClassChecker valid_class_checker, const std::string& names_file)
    : min_conf_(min_conf), iou_thresh_(iou_thresh) {
    net_ = cv::dnn::readNetFromONNX(model_path);

    selectDNNBackendAndTarget(net_);

    assert(!net_.empty() && "Failed to load ONNX model");

    // Ŭ���� ����
    std::string resolved_names_file = names_file;
    if (resolved_names_file.empty()) {
        size_t last_slash_idx = model_path.find_last_of("\\/");
        // std::string::npos - -1 ���� ������ ����� ���ڿ��� ���� �� ��ȯ
        if (std::string::npos != last_slash_idx) {
            resolved_names_file = model_path.substr(0, last_slash_idx + 1) + "coco.names";
        }
        else {
            resolved_names_file = "coco.names";
        }
    }

    input_size_.width = 320;
    input_size_.height = 320;

    loadClassNames(resolved_names_file);

    // ���׿�����
    valid_class_checker_ = valid_class_checker ? valid_class_checker : [](int, const std::string&) { return true; };
}

cv::Mat Yolo11::preprocess(const cv::Mat& image) {
    // resize + blob �� ���� ó��
    cv::Mat blob;
    cv::dnn::blobFromImage(image, blob, 1.0 / 255.0, input_size_, cv::Scalar(), true, false, CV_32F);

    return blob;
}

void Yolo11::postprocess(const cv::Mat& output, const cv::Size& original_size, DetectionResult* results, int* resultCount) {
    assert(output.dims == 2 && output.cols > 0 &&
        output.rows == (4 + class_names_.size()) &&
        "Invalid output shape");

    // suppressed������ output.cols ũ�⸸ŭ false�� ���� �ʱ�ȭ
    std::vector<bool> suppressed(output.cols, false);
    // cv::Point2f - 2D ��ǥ�� �����ϴ� ����ü. (x,y) ���� float Ÿ������ ����
    // ���� ũ��� �Է� ũ���� ������ ����Ͽ� ����
    cv::Point2f scale(
        static_cast<float>(original_size.width) / input_size_.width,
        static_cast<float>(original_size.height) / input_size_.height
    );

    // �ִ� confidence ã��
    std::vector<std::tuple<float, int, int>> conf_idx_class_pairs;
    for (int i = 0; i < output.cols; ++i) {
        // 0~3: �ٿ�� �ڽ� ��ǥ(x, y, w, h)
        // 4~N: �� Ŭ������ Ȯ��(scores)
        // ���� ���� conf�� ���� Ŭ���� ã��
        cv::Mat scores = output.rowRange(4, output.rows).col(i);
        double max_conf;
        cv::Point max_loc;
        // scores ��Ŀ��� �ִ밪(max_conf)�� �ش� ��ġ(max_loc.y)�� ã��
        cv::minMaxLoc(scores, nullptr, &max_conf, nullptr, &max_loc);
        int class_id = max_loc.y;

        conf_idx_class_pairs.emplace_back(max_conf, i, class_id);
    }

    // conf�� ���� ��ü���� �������� ����
    std::sort(conf_idx_class_pairs.begin(), conf_idx_class_pairs.end(), 
        [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });

    int count = 0;
    // NMS - ��ü Ž��(Object Detection) �𵨿��� �ߺ��� �ٿ�� �ڽ��� �����ϴ� �˰���
    for (size_t i = 0; i < conf_idx_class_pairs.size(); ++i) {
        auto [conf1, idx1, class_id1] = conf_idx_class_pairs[i];
        if (suppressed[idx1]) continue;

        if (!valid_class_checker_(class_id1, class_names_[class_id1]) || conf1 < min_conf_) continue;

        /*float cx = output.at<float>(0, idx1);
        float cy = output.at<float>(1, idx1);*/
        results[count].cx = output.at<float>(0, idx1);
        results[count].cy = output.at<float>(1, idx1);
        results[count].width = output.at<float>(2, idx1);
        results[count].height = output.at<float>(3, idx1);
        results[count].confidence = conf1;
        results[count].classId = class_id1;

        getRectPosition(results[count], scale.x, scale.y);

        count++;

        // NMS ����
        for (size_t j = i + 1; j < conf_idx_class_pairs.size(); ++j) {
            auto[conf2, idx2, class_id2] = conf_idx_class_pairs[j];
            if (suppressed[idx2]) continue;

            // (5) Ŭ������ �ٸ��� NMS skip
            if (class_id1 != class_id2) continue;

            DetectionResult temp;
            /*float cx = output.at<float>(0, idx2);
            float cy = output.at<float>(1, idx2);*/
            temp.cx = output.at<float>(0, idx2);
            temp.cy = output.at<float>(1, idx2);
            temp.width = output.at<float>(2, idx2);
            temp.height = output.at<float>(3, idx2);
            temp.confidence = conf2;
            temp.classId = class_id2;

            getRectPosition(temp, scale.x, scale.y);

            // �� �ڽ��� IoU(��ġ�� ����)�� ������ �Ӱ谪���� ũ�ٸ� �ߺ��� ������ �����ϰ� ����
            if (calculateIoU(results[i], temp, scale) > iou_thresh_) {
                suppressed[idx2] = true;
            }
        }
    }

    *resultCount = count;
}

cv::Mat Yolo11::detect(const cv::Mat& image) {
    assert(!image.empty() && image.type() == CV_8UC3 && "Invalid input image");

    cv::Mat blob = preprocess(image);

    net_.setInput(blob);
    std::vector<cv::Mat> outputs;

    // net_.getUnconnectedOutLayersNames() - ��� ���̾�(������ ���̾�)�� �̸� ��ȯ
    net_.forward(outputs, net_.getUnconnectedOutLayersNames());
    assert(outputs.size() == 1 && "Unexpected number of outputs");
    // N x (4 + C)
    // N: Ž���� �ٿ�� �ڽ� ��
    // 4: �ٿ�� �ڽ� ��ǥ(�� : [x, y, w, h] �Ǵ�[x1, y1, x2, y2])
    // C: Ŭ���� Ȯ��(Ŭ���� ������ �ش�)
    // 1���� ��ü�� (4 + Ŭ���� ����) ���� ���� ������ ��� ����
    cv::Mat rawOutput = outputs[0].reshape(0, 4 + class_names_.size());

    return rawOutput;
}

std::map<int, std::string> Yolo11::getClassIdNamePairs() const {
    return class_names_;
}