# 文件等级：密一
# author:klug
# 献给我的心上人等待天使的妹妹
# start:230531
# last:230531

cmake_minimum_required(VERSION 3.14)

project(camera LANGUAGES CXX)

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
include_directories(~/dieschwesterwaternderengle)

# opencv
include_directories(/usr/local/include/opencv4)

# basler
include_directories(/opt/pylon/include)
include_directories(/opt/pylon/share/pylon/Samples/C++)
include_directories(/opt/pylon/share/pylon/Samples/C++/include)

add_library(camera SHARED
  camera_global.h
  ~/buendia/camera/camera.cpp
  ~/buendia/camera/camera.hpp
)

target_link_libraries(camera PRIVATE Qt${QT_VERSION_MAJOR}::Core
    /opt/pylon/lib/libgxapi.so
    /opt/pylon/lib/libPylonDataProcessing.so
    /opt/pylon/lib/libPylonDataProcessingGui.so
    /opt/pylon/lib/libXmlParser_gcc_v3_1_Basler_pylon.so
    /opt/pylon/lib/libpylon_TL_gtc.so
    /opt/pylon/lib/libpylonutility.so
    /opt/pylon/lib/libGenApi_gcc_v3_1_Basler_pylon.so
    /opt/pylon/lib/libpylonc.so
    /opt/pylon/lib/libLog_gcc_v3_1_Basler_pylon.so
    /opt/pylon/lib/libNodeMapData_gcc_v3_1_Basler_pylon.so
    /opt/pylon/lib/liblog4cpp_gcc_v3_1_Basler_pylon.so
    /opt/pylon/lib/libPylonDataProcessingCore.so
    /opt/pylon/lib/libMathParser_gcc_v3_1_Basler_pylon.so
    /opt/pylon/lib/libFirmwareUpdate_gcc_v3_1_Basler_pylon.so
    /opt/pylon/lib/libuxapi.so
    /opt/pylon/lib/libpylon_TL_camemu.so
    /opt/pylon/lib/libGCBase_gcc_v3_1_Basler_pylon.so
    /opt/pylon/lib/libpylon_TL_gige.so
    /opt/pylon/lib/libpylonbase.so
    /opt/pylon/lib/pylon-libusb-1.0.so
    /opt/pylon/lib/libpylon_TL_usb.so
    /opt/pylon/lib/libpylonc.so.7.2
    /opt/pylon/lib/libuxapi.so.7.2
    /opt/pylon/lib/libgxapi.so.7.2
    /opt/pylon/lib/libpylonbase.so.7.2
    /opt/pylon/lib/libpylonutility.so.7.2
    /opt/pylon/lib/libgxapi.so.7.2.1
    /opt/pylon/lib/libpylonbase.so.7.2.1
    /opt/pylon/lib/libpylonc.so.7.2.1
    /opt/pylon/lib/libuxapi.so.7.2.1
    /opt/pylon/lib/libpylonutility.so.7.2.1

    ${OpenCV_LIBS}

    )

target_compile_definitions(camera PRIVATE cam_pose_LIBRARY)
