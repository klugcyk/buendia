/*
    author:klug
    献给我的心上人等待天使的妹妹
    start:221129
    last:230105
*/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <thread>
#include <future>
#include <chrono>
#include <stdio.h>
#include <unistd.h>

#define use_opencv_camera // use the opencv camera
//#define use_pylon_camera // use the pylon camera
//#define cal_print // print the data in the calibration process

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

#ifdef use_opencv_camera
    #define Bruder_camera_ID 0
    #define Schwester_camera_ID 0

#endif

extern std::mutex mtx;

class camera_
{
public:
    camera_();
    ~camera_();
    friend class MainWindow;
public:
    cv::Mat cam_img_l;
    cv::Mat cam_img_r;
    cv::Mat Bruder_img;
    cv::Mat Schwester_img;
    cv::Mat cam_img_grab;
    cv::Mat img_continue;
    cv::Mat cor_img;
    int set_camera_fps=10;
    int set_offset_x=0;
    int set_offset_y=0;
    int set_height=0;
    int set_width=0;
    int set_exposure_time=10; //us

    int read_camera_fps=6174;
    int read_offset_x=0;
    int read_offset_y=0;
    int read_height=6174;
    int read_width=6174;
    int read_exposure_time=6174; //us

    int max_width=2000;
    int max_height=2000;

private:
    std::vector<cv::Mat> transform_matrix; //4*4 external parameter
    std::vector<cv::Mat> Bruder_transform_matrix; //4*4 external parameter
    std::vector<cv::Mat> Schwester_transform_matrix; //4*4 external parameter
    //std::vector<cv::Mat> external_Schwester;
    //std::vector<cv::Mat> external_Bruder;
    Size board_size=cv::Size(9,6);
    cv::Mat cameraMatrix=cv::Mat(3,3,CV_32FC1,Scalar::all(0));
    cv::Size2f square_size=Size2f(14.25,14.25); //real length of board=14.25
    cv::Mat distCoeffs=Mat(1,5,CV_32FC1,Scalar::all(0));
    std::vector<cv::Mat> rvecsMat;
    std::vector<cv::Mat> rotation_matrix;//3*3
    std::vector<cv::Mat> tvecsMat;
    std::vector<Mat> Schwester_rvecsMat;
    std::vector<cv::Mat> Schwester_rotation_matrix;
    std::vector<cv::Mat> Schwester_tvecsMat;
    std::vector<cv::Mat> external_Bruder;

    std::vector<cv::Mat> Bruder_rotation_matrix;
    std::vector<cv::Mat> Bruder_rvecsMat;
    std::vector<cv::Mat> Bruder_tvecsMat;
    std::vector<cv::Mat> external_Schwester;

    std::vector<cv::Mat> Schwinge_Matrix;

    //std::thread test_thread;

#ifdef use_opencv_camera
    cv::VideoCapture opencv_camera;

#endif

private:

protected:
    void calculate_relationship();
#ifdef use_pylon_camera
    void camera_set_parameter();
    void camera_read_parameter();
    void zwei_camera_grab();

#endif

#ifdef use_opencv_camera
    void img_grab_opencv();
    void camera_set_parameter_opencv();
    void camera_read_parameter_opencv();
    void img_offset(Mat &input,Mat &output);
    bool first_read_flag=1;

#endif

    void calculate_relationship(std::vector<cv::Mat> external_Schwester,std::vector<cv::Mat> external_Bruder);//not achieve
    std::vector<cv::Mat> camera_calibrate(std::vector<cv::Mat> img_vector);
    cv::Mat Schwinge=cv::Mat(4,4,CV_32FC1);
    bool parameter_update_flag=0;
    //void test();
};

#endif // CAMERA_HPP
