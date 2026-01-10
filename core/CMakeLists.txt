cmake_minimum_required(VERSION 3.10)
project(cvision LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CVISION_ENABLE_GPROF "Enable -pg instrumentation for gprof" OFF)

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

# Threading Building Blocks (TBB)
find_package(TBB REQUIRED tbb)
message(STATUS "--- Intel TBB Diagnostic ---")
message(STATUS "TBB include directories: ${TBB_INCLUDE_DIRS}")
message(STATUS "TBB libraries: ${TBB_IMPORTED_TARGETS}")

# Create executable FIRST
add_executable(cvision 
    cli/cvision_cli.cpp
    src/cvision.cpp
    src/levenshtein_search.cpp
    src/yolo_detector.cpp
    src/yolo_model.cpp
)

# Include headers directories and link libraries
target_include_directories(cvision PRIVATE 
    ${OpenCV_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/cli
)
target_link_libraries(cvision PRIVATE ${OpenCV_LIBRARIES} nlohmann_json::nlohmann_json TBB::tbb)

if(CVISION_ENABLE_GPROF)
    message(STATUS "gprof instrumentation enabled (-pg)")
    target_compile_options(cvision PRIVATE -pg)
    target_link_options(cvision PRIVATE -pg)
endif()

install(TARGETS cvision DESTINATION bin)