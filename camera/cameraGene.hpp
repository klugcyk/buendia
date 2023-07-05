/*
    author:klug
    献给我的心上人等待天使的妹妹
    start:221129
    last:230704
*/

#ifndef CAMERAGENE_HPP
#define CAMERAGENE_HPP

#include <opencv2/opencv.hpp>
#include <thread>
#include <future>
#include <chrono>
#include <stdio.h>
#include <unistd.h>
#include "camera/camera.hpp"

using namespace cv;
using namespace std;

//相机通用操作库，常用的相机算法
class cameraGene
{
public:
    cameraGene();
    cameraGene(int xSize,int ySize,float xLength,float yLength); //标定参数初始化
    ~cameraGene();
    void cameraCalibrate(std::vector<cv::Mat> img_vector);
    void cameraCalibrate(std::vector<cv::Mat> img_vector,cv::Mat &camera_matrix,cv::Mat &dis);

public:
    cv::Mat cameraMatrix=cv::Mat(3,3,CV_32FC1,cv::Scalar::all(0)); //相机内参
    cv::Mat distCoeffs=cv::Mat(1,5,CV_32FC1,cv::Scalar::all(0)); //畸变系数
    std::vector<cv::Mat> extrinsic;
    cv::Size board_size=cv::Size(6,9); //标定板的尺度
    cv::Size2f square_size=cv::Size2f(6.96,6.96); //标定板的格子大小 3.95
    std::vector<cv::Mat> rvecsMat;
    std::vector<cv::Mat> rotation_matrix;
    std::vector<cv::Mat> tvecsMat;

};

#endif // CAMERAGENE_HPP
