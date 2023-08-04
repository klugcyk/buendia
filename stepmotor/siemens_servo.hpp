/*
    文件等级：密一
    author:klug
    献给我的心上人等待天使的妹妹
    start:221129
    last:230323
*/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <thread>
#include <future>
#include <chrono>
#include <stdio.h>
#include <unistd.h>

#define use_pylon_camera // use the pylon camera
#define cal_print // print the data in the calibration process
#define use_laserscan_camera //使用扫描相机

#define camera_print_msg_info
#define camera_print_data_info
#define camera_print_error_info

#ifdef use_pylon_camera
#include <pylon/BaslerUniversalInstantCameraArray.h>
#include <pylon/usb/BaslerUsbInstantCameraArray.h>
#include <pylon/PylonIncludes.h>

#endif

using namespace cv;
using namespace std;

#ifdef use_pylon_camera
using namespace Basler_UniversalCameraParams;
using namespace Pylon;

#endif

extern std::mutex mtx;

#ifdef use_pylon_camera

class camera_
{
public:
    camera_();
    ~camera_();

public:
    cv::Mat cam_img;
    cv::Mat cam_img_grab;
    cv::Mat img_continue;
    cv::Mat cor_img;
    int set_camera_fps=10;
    int set_height=0;
    int set_width=0;
    int set_exposure_time=10; //us

    int read_camera_fps=6174;
    int read_height=6174;
    int read_width=6174;
    int read_exposure_time=6174; //us

    int max_width=2000;
    int max_height=2000;

protected:
    void camera_set_parameter();
    void camera_read_parameter();
    void camera_grab();

};

#endif

//扫描激光测振仪的相机控制
class camera_laserscan
{
public:
    camera_laserscan();
    ~camera_laserscan();

public:

protected:

protected:

private:

private:

};


#endif // CAMERA_HPP
