/*
    文件等级：密一
    author:klug
    献给不喜欢我的弗雷德里希冯海因洛特
    start:230323
    last:230605
*/

#ifndef galvo_control_HPP
#define galvo_control_HPP

#include <iostream>

#include "socket/socket.hpp"
#include <opencv2/opencv.hpp>
#include "galvo_control/Crc16Class.h"
#include "math/geometry.hpp"

#ifdef construct_cal_HPP
#include "img_process/construct_cal.hpp"
#endif

#define galvo_ip "192.168.1.11"
#define galvo_port 8088

#define galvo_control_msg_print_info
#define galvo_control_data_print_info
#define galvo_control_error_print_info

#ifdef construct_cal_HPP
using namespace calibrate_construct;
#endif



class galvo_control:public socket_
{
public:
    galvo_control();
    ~galvo_control();
    int galvo_rotate(float angle_ein,float angle_zwei); //控制两个振镜旋转
    void galvo_read(); //读取两个振镜角度

public:
    math_geometry::geo_line_param axia_ein; //第一振镜旋转轴线
    math_geometry::geo_line_param axia_zwei; //第二振镜旋转轴线
    math_geometry::geo_plane_param ein_galvo_plane; //第一振镜平面参数，0度
    math_geometry::geo_plane_param zwei_galvo_plane; //第二振镜平面参数，0度
    float ein_galvo_min=-12.5;
    float ein_galvo_max=12.5;
    float zwei_galvo_min=-12.5;
    float zwei_galvo_max=12.5;
    int digital_galvo_min=-32700;
    int digital_galvo_max=32700;
    float ein_line_k;
    float ein_line_b;
    float zwei_line_k;
    float zwei_line_b;
    int socket_accept;//振镜通讯tcp编号
    cv::Point3f tran;
    float ein_galvo_angle=6174; //第一振镜的角度
    float zwei_galvo_angle=6174; //第二振镜的角度

};


#endif // galvo_control_HPP
