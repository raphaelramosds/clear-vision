cmake_minimum_required(VERSION 3.10)
project(ClearVisionVideoProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Add main executable
add_executable(video-processor 
    main.cpp
    yolo-detector.cpp
    video-processor.cpp
)

# Link libraries
target_link_libraries(video-processor PRIVATE ${OpenCV_LIBS})

# Include directories
target_include_directories(video-processor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

# Installation
install(TARGETS video-processor DESTINATION bin)
