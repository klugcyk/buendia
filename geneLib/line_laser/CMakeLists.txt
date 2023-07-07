# 文件等级：密一
# author:klug
# 献给杜尔西内娅德尔托博索
# start:230617
# last:230617

cmake_minimum_required(VERSION 3.14)

project(linelaser LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# pcl
find_package(PCL REQUIRED)
# opencv
find_package(OpenCV REQUIRED)
# eigen
find_package(Eigen3 REQUIRED)
# selbst
include_directories(~/derbuder)

# opencv
include_directories(/usr/local/include/opencv4)

# pcl
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

# eigen
include_directories(${EIGEN3_INCLUDE_DIRS})

# VTK
find_package(VTK REQUIRED)
include_directories(/usr/include/vtk-7.1)

add_library(linelaser SHARED
  linelaser_global.h
  ~/derbruder/linelaser.cpp
  ~/derbruder/linelaser.hpp
)

target_link_libraries(linelaser PRIVATE Qt${QT_VERSION_MAJOR}::Core
    ${OpenCV_LIBS}

    ${PCL_LIBRARIES}

    ${VTK_LIBRARIES}
    
    )

target_compile_definitions(linelaser PRIVATE CONSTRUCT_LIBRARY)
