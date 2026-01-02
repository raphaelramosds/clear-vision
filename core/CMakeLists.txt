cmake_minimum_required(VERSION 3.10)
project(ClearVisionDetect LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenCV library check
find_package(OpenCV 4 REQUIRED)
message(STATUS "--- OpenCV Diagnostic ---")
message(STATUS "Found OpenCV version: ${OpenCV_VERSION}")
message(STATUS "Include directories:   ${OpenCV_INCLUDE_DIRS}")
message(STATUS "Libraries to link:    ${OpenCV_LIBRARIES}")

if(TARGET opencv_cudaobjdetect OR TARGET opencv_cudaimgproc)
    message(STATUS "OpenCV with CUDA support FOUND")
else()
    message(WARNING "OpenCV without CUDA support NOT FOUND. CPU-only processing will be used.")
endif()

# JSON library check
find_package(nlohmann_json 3.2.0 REQUIRED)
message(STATUS "--- JSON for Modern C++ Diagnostic ---")
message(STATUS "nlohmann_json version: ${nlohmann_json_VERSION}")

# Create executable FIRST
add_executable(DetectModule 
    src/detect.cpp
    src/yolo-detector.cpp
    src/video-processor.cpp
    src/word2vec-search.cpp
)

# THEN configure it
target_include_directories(DetectModule PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(DetectModule PRIVATE ${OpenCV_LIBRARIES} nlohmann_json::nlohmann_json)

install(TARGETS DetectModule DESTINATION bin)