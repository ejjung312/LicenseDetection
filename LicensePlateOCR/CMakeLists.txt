cmake_minimum_required(VERSION 3.10)
project(LicenseDetectionOCR)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    set(OpenCV_DIR "C:/opencv/build")
    find_package(OpenCV REQUIRED)

    # Paddle Inference 설치 경로
    set(PADDLE_DIR C:/paddle_inference)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OPENCV REQUIRED opencv4)
endif()

# 소스 및 헤더 파일 목록 (수동 지정)
set(SOURCES
    license_plate_ocr.cpp
    paddle_ocr.cpp
)

set(HEADERS
    license_plate_ocr.h
    paddle_ocr.h
)

# DLL, so 파일 생성
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

if (WIN32)
    include_directories(${OpenCV_INCLUDE_DIRS})

    include_directories(
        ${PADDLE_DIR}/paddle/include,
        ${OpenCV_LIBS}
    )

    include_directories(${PROJECT_SOURCE_DIR}/include)
    file(GLOB SRCS src/*.cpp)

    add_executable(ocr_rec  ${SRCS})

    target_link_libraries(${PROJECT_NAME} ${PADDLE_DIR}/paddle/lib/libpaddle_inference.lib)
elseif(UNIX)
    include_directories(${OpneCV_INCLUDE_DIRS})
    link_directories(${OpenCV_LIBRARY_DIRS})
    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})
endif()