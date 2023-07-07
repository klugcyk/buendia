# 文件等级：密一
# author:klug
# 献给我的心上人等待天使的妹妹
# start:230628
# last:230628

cmake_minimum_required(VERSION 3.14)

project(cameraGene LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

# opencv
find_package(OpenCV REQUIRED)

# selbst
include_directories(~/buendia)

# opencv
include_directories(/usr/local/include/opencv4)

add_library(cameraGene SHARED
  cameraGene_global.h
  ~/buendia/camera/cameraGene1.cpp
  #~/buendia/camera/cameraGene.hpp
)

target_link_libraries(cameraGene #PRIVATE Qt${QT_VERSION_MAJOR}::Core 

	${OpenCV_LIBS} 
)

target_compile_definitions(cameraGene PRIVATE cameraGene_LIBRARY)
