# 文件等级：密一
# author:klug
# 献给我的心上人等待天使的妹妹
# start:230626
# last:230626

cmake_minimum_required(VERSION 3.14)

project(gene_operate LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

# selbst
include_directories(~/buendia)
include_directories(~/dieschwesterwaternderengle)

add_library(gene_operate SHARED
  gene_operate_global.h
  ~/buendia/gene_operate/json_operate.cpp
  ~/buendia/gene_operate/json_operate.hpp
 # ~/buendia/gene_operate/xml_operate.cpp
 # ~/buendia/gene_operate/xml_operate.hpp
)

target_link_libraries(gene_operate PRIVATE Qt${QT_VERSION_MAJOR}::Core


    )

target_compile_definitions(gene_operate PRIVATE gene_operate_LIBRARY)
