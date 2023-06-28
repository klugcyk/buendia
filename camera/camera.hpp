/*
    文件等级：密一
    author:klug
    献给我的心上人等待天使的妹妹
    start:221129
    last:230628
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

#define camera_print_msg_info
#define camera_print_data_info
#define camera_print_error_info
#define camera_save
#define camera_continue 1 //开启相机连续采集 1

#ifdef use_pylon_camera
#include <pylon/BaslerUniversalInstantCameraArray.h>
#include <pylon/usb/BaslerUsbInstantCameraArray.h>
#include <pylon/PylonIncludes.h>
#include "ConfigurationEventPrinter.h"
#include "CameraEventPrinter.h"

#endif

using namespace cv;
using namespace std;

#ifdef use_pylon_camera
using namespace Basler_UniversalCameraParams;
using namespace Pylon;

#endif

extern std::mutex mtx;

#ifdef use_pylon_camera

class basler_camera:public CImageEventHandler/*继承用于连续采集图片*/
{
public:
    basler_camera();
    basler_camera(int mod);
    ~basler_camera();
    void camera_set_parameter();
    void camera_read_parameter();
    void camera_set_parameter_zwei();//读取连个相机的参数
    void camera_read_parameter_zwei();
    void camera_grab_gray();
    void camera_grab_zwei();
    void camera_grab_rgb();
    cv::Mat camera_grab_gray_return();
    cv::Mat camera_grab_rgb_return();
    void camera_video_offnen();

public:    
    cv::Mat cam_img;
    cv::Mat cam_img_grab;
    cv::Mat cam_img_continue;
    cv::Mat img_continue;
    cv::Mat cor_img;
    int grab_mod=0;
    int set_camera_fps=10;
    int set_height=0;
    int set_width=0;
    int set_exposure_time=10; //us
    int read_camera_fps=6174;
    int read_height=6174;
    int read_width=6174;
    int read_exposure_time=6174; //us
    int camera_exposure_time_link=6174; //us
    int camera_exposure_time_richt=6174; //us
    int max_width=2000;
    int max_height=2000;
    cv::Mat camera_grab_img;
    int cam_width=2592;
    int cam_height=2048;
    uchar *first_pixel;
    std::mutex continue_lock;
#if camera_continue==1
    std::thread continue_img_thread; //连续采集图片线程
    bool continue_achieve_flag=0;//图像连续采集完成标志位

    //std::unique_lock<std::mutex> cam_lock(continue_lock);
#endif

};

#endif

#endif // CAMERA_HPP
