cmake_minimum_required(VERSION 3.10)
project(LicenseDetection)

set(CMAKE_CXX_STANDARD 17)

# OpenCV 경로 설정 (자동 감지 시도)
if(WIN32)
    set(OpenCV_DIR "C:/opencv/build")
    find_package(OpenCV REQUIRED)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OPENCV REQUIRED opencv4)
endif()

# 소스 및 헤더 파일 목록 (수동 지정)
set(SOURCES
    common_functions.cpp
    license_plate_detection.cpp
    yolo11.cpp
)

set(HEADERS
    common_functions.h
    detection_result.h
    license_plate_detection.h
    yolo11.h
)

# DLL 파일 생성
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

if (WIN32)
    include_directories(${OpenCV_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
elseif(UNIX)
    include_directories(${OpneCV_INCLUDE_DIRS})
    link_directories(${OpenCV_LIBRARY_DIRS})
    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})
endif()