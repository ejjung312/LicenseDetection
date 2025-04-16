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
        ' ' (스페이스)
        '\t' (탭)
        '\n' (개행)
        '\r' (캐리지 리턴)
        '\f' (폼 피드)
        '\v' (수직 탭)
        */
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v")); // 공백이 아닌 첫번째 문자열 위치를 찾고 찾은 인덱스로 잘라내어 공백 제거
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1); // 공백이 아닌 마지막 문자의 위치를 찾아 공백이 끝나는 다음 위치(+1)부터 문자열 끝까지 삭제

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

    // 클래스 파일
    std::string resolved_names_file = names_file;
    if (resolved_names_file.empty()) {
        size_t last_slash_idx = model_path.find_last_of("\\/");
        // std::string::npos - -1 값을 가지는 상수로 문자열이 없을 때 반환
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

    // 삼항연산자
    valid_class_checker_ = valid_class_checker ? valid_class_checker : [](int, const std::string&) { return true; };
}

cv::Mat Yolo11::preprocess(const cv::Mat& image) {
    // resize + blob 한 번에 처리
    cv::Mat blob;
    cv::dnn::blobFromImage(image, blob, 1.0 / 255.0, input_size_, cv::Scalar(), true, false, CV_32F);

    return blob;
}

void Yolo11::postprocess(const cv::Mat& output, const cv::Size& original_size, DetectionResult* results, int* resultCount) {
    assert(output.dims == 2 && output.cols > 0 &&
        output.rows == (4 + class_names_.size()) &&
        "Invalid output shape");

    // suppressed변수에 output.cols 크기만큼 false로 벡터 초기화
    std::vector<bool> suppressed(output.cols, false);
    // cv::Point2f - 2D 좌표를 저장하는 구조체. (x,y) 값을 float 타입으로 저장
    // 원본 크기와 입력 크기의 비율을 계산하여 저장
    cv::Point2f scale(
        static_cast<float>(original_size.width) / input_size_.width,
        static_cast<float>(original_size.height) / input_size_.height
    );

    // 최대 confidence 찾기
    std::vector<std::tuple<float, int, int>> conf_idx_class_pairs;
    for (int i = 0; i < output.cols; ++i) {
        // 0~3: 바운딩 박스 좌표(x, y, w, h)
        // 4~N: 각 클래스의 확률(scores)
        // 가장 높은 conf를 가진 클래스 찾기
        cv::Mat scores = output.rowRange(4, output.rows).col(i);
        double max_conf;
        cv::Point max_loc;
        // scores 행렬에서 최대값(max_conf)과 해당 위치(max_loc.y)를 찾음
        cv::minMaxLoc(scores, nullptr, &max_conf, nullptr, &max_loc);
        int class_id = max_loc.y;

        conf_idx_class_pairs.emplace_back(max_conf, i, class_id);
    }

    // conf가 높은 객체부터 내림차순 정렬
    std::sort(conf_idx_class_pairs.begin(), conf_idx_class_pairs.end(), 
        [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });

    int count = 0;
    // NMS - 객체 탐지(Object Detection) 모델에서 중복된 바운딩 박스를 제거하는 알고리즘
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

        // NMS 실행
        for (size_t j = i + 1; j < conf_idx_class_pairs.size(); ++j) {
            auto[conf2, idx2, class_id2] = conf_idx_class_pairs[j];
            if (suppressed[idx2]) continue;

            // (5) 클래스가 다르면 NMS skip
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

            // 두 박스의 IoU(겹치는 비율)가 설정된 임계값보다 크다면 중복된 것으로 간주하고 제거
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

    // net_.getUnconnectedOutLayersNames() - 출력 레이어(마지막 레이어)의 이름 반환
    net_.forward(outputs, net_.getUnconnectedOutLayersNames());
    assert(outputs.size() == 1 && "Unexpected number of outputs");
    // N x (4 + C)
    // N: 탐지된 바운딩 박스 수
    // 4: 바운딩 박스 좌표(예 : [x, y, w, h] 또는[x1, y1, x2, y2])
    // C: 클래스 확률(클래스 개수에 해당)
    // 1개의 객체당 (4 + 클래수 개수) 개의 값을 갖도록 행렬 변형
    cv::Mat rawOutput = outputs[0].reshape(0, 4 + class_names_.size());

    return rawOutput;
}

std::map<int, std::string> Yolo11::getClassIdNamePairs() const {
    return class_names_;
}