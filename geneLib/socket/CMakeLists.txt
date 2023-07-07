# 文件等级：密一
# author:klug
# 献给我的心上人等待天使的妹妹
# start:230607
# last:230607

cmake_minimum_required(VERSION 3.14)

project(socket LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

# opencv
find_package(OpenCV REQUIRED)

# selbst
include_directories(~/buendia)

# opencv
include_directories(/usr/local/include/opencv4)

# basler
include_directories(/opt/pylon/include)
include_directories(/opt/pylon/share/pylon/Samples/C++)

add_library(socket SHARED
  socket_global.h
  ~/buendia/socket/socket.cpp
  ~/buendia/socket/socket.hpp
)

target_link_libraries(socket PRIVATE Qt${QT_VERSION_MAJOR}::Core
    

    )

target_compile_definitions(socket PRIVATE socket_LIBRARY)
