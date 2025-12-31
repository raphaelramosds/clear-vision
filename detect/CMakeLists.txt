cmake_minimum_required(VERSION 3.10)
project(ClearVisionVideoProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 4 REQUIRED)

message(STATUS "--- OpenCV Diagnostic ---")
message(STATUS "Found OpenCV version: ${OpenCV_VERSION}")
message(STATUS "Include directories:  ${OpenCV_INCLUDE_DIRS}")
message(STATUS "Libraries to link:    ${OpenCV_LIBRARIES}")

if(TARGET opencv_cudaobjdetect OR TARGET opencv_cudaimgproc)
    message(STATUS "OpenCV WITH CUDA SUPPORT FOUND")
else()
    message(WARNING "OpenCV WITHOUT CUDA SUPPORT FOUND")
endif()

add_executable(video-processor 
    main.cpp
    yolo-detector.cpp
    video-processor.cpp
)

target_include_directories(video-processor PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(video-processor PRIVATE ${OpenCV_LIBRARIES})

install(TARGETS video-processor DESTINATION bin)