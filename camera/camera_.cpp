/*
    author:klug
    献给我的心上人等待天使的妹妹
    start:221129
    last:230104
*/

#include "camera.hpp"
#include <chrono>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <time.h>
#include <stdlib.h>

#ifdef use_pylon_camera
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/BaslerUniversalInstantCameraArray.h>
#include <pylon/Info.h>
#include <pylon/usb/BaslerUsbInstantCameraArray.h>
using namespace Pylon;
using namespace Basler_UniversalCameraParams;
using namespace GenApi;
using namespace Basler_UsbCameraParams;

#endif

using namespace std;
using namespace cv;

//static const size_t c_maxCamerasToUse =2;

camera_::camera_()
{
    printf("construct tha camera class...\n");
#ifdef use_pylon_camera
    PylonInitialize();
#endif
    //test_thread=std::thread(&camera_::img_grab_opencv,this);
}

camera_::~camera_()
{
#ifdef use_pylon_camera
    PylonTerminate();
#endif
    //test_thread.join();
}
/*
void camera_::test()
{
    //int i=0;
    //while(1)
    //{
    //    i++;
    //    zwei_camera_grab();
    //    printf("time:=%d",i);
    //    sleep(5);
    //}
}*/

std::vector<cv::Mat> camera_::camera_calibrate(std::vector<cv::Mat> img_vector)
{
    printf("start the calibrate from img_array...\n");

    int image_count = 0;
    Size image_size;

    vector<Point2f> image_points;
    vector<vector<Point2f>> image_points_seq;

    for(size_t i=0;i<img_vector.size();i++)
    {
        image_count++;

        //cout << "image_count = " << image_count << endl;
        Mat imageInput = img_vector[i];
        if (image_count == 1)
        {
            //image_size.width = imageInput.cols;
            //image_size.height = imageInput.rows;
            //cout << "image_size.width = " << image_size.width << endl;
            //cout << "image_size.height = " << image_size.height << endl;
        }

        bool bRes = findChessboardCorners(imageInput, board_size, image_points, 0);
        if (bRes)
        {
            Mat view_gray;
            //cout << "imageInput.channels()=" << imageInput.channels() << endl;
            cvtColor(imageInput, view_gray, cv::COLOR_RGB2GRAY);

            cv::cornerSubPix(view_gray, image_points, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.01));

            image_points_seq.push_back(image_points);
            drawChessboardCorners(view_gray, board_size, image_points, true);
        }
        else
        {
            printf("fail...\n");
        }
    }

    vector<vector<Point3f>> object_points_seq;

    for(int t=0;t<image_count;t++)
    {
        vector<Point3f> object_points;
        for (int i = 0; i < board_size.height; i++)
        {
            for (int j = 0; j < board_size.width; j++)
            {
                Point3f realPoint;
                realPoint.x = i * square_size.width;
                realPoint.y = j * square_size.height;
                realPoint.z = 0;
                object_points.push_back(realPoint);
            }
        }
        object_points_seq.push_back(object_points);
    }

    double rms = calibrateCamera(object_points_seq, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, cv::CALIB_FIX_K3 + cv::CALIB_ZERO_TANGENT_DIST);
    cout << "RMS:" << rms << "pixel" << endl << endl;
#ifdef cal_print
    for(size_t i=0;i<tvecsMat.size();i++)
    {
        std::cout<<"transform "
              <<i
              <<" :="
              <<tvecsMat[i]
              <<std::endl;
    }
#endif
    rotation_matrix.clear();
    for(size_t i=0;i<rvecsMat.size();i++)
    {
#ifdef cal_print
        std::cout<<"rotate vector:="
              <<i
              <<" :="
              <<rvecsMat[i]
              <<std::endl;
#endif
        Mat rm = Mat(3,3,CV_64FC1,Scalar::all(0));
        Rodrigues(rvecsMat[i], rm);
        rotation_matrix.push_back(rm);
#ifdef cal_print
        std::cout<<"rotate matriz "
              <<i
              <<" :="
              <<rm
              <<std::endl;
#endif
    }

    transform_matrix.clear();
    for(size_t i=0;i<rotation_matrix.size();i++)
    {
        Mat tm=cv::Mat(4,4,CV_64FC1,Scalar::all(0));
        for(int c=0;c<3;c++)
        {
            for(int r=0;r<3;r++)
            {
                tm.at<double>(c,r)=rotation_matrix[i].at<double>(c,r);
            }
        }
        tm.at<double>(0,3)=tvecsMat[i].at<double>(0,0);
        tm.at<double>(1,3)=tvecsMat[i].at<double>(0,1);
        tm.at<double>(2,3)=tvecsMat[i].at<double>(0,2);
        tm.at<double>(3,3)=1;
        transform_matrix.push_back(tm);
#ifdef cal_print
        std::cout<<"tm "
                <<i
                <<tm
                <<endl;
#endif
    }

    printf("calibrate done from img_array...\n");
    return transform_matrix;
}

#ifdef use_pylon_camera

void camera_::zwei_camera_grab()
{
    try
    {
        CTlFactory& tlFactory = CTlFactory::GetInstance();

        DeviceInfoList_t devices;
        if (tlFactory.EnumerateDevices( devices ) == 0)
        {
            throw RUNTIME_EXCEPTION("No camera present.");
        }

        CInstantCameraArray cameras( min( devices.size(), c_maxCamerasToUse ) );
        for (size_t i = 0; i < cameras.GetSize(); ++i)
        {
            cameras[i].Attach( tlFactory.CreateDevice( devices[i] ) );
            cout << "Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
        }

        CGrabResultPtr ptrGrabResult;
        CPylonImage pylonImage;
        CImageFormatConverter cov;

        std::unique_lock<std::mutex> lock(mtx);
        cameras[0].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded())
        {
            cov.Convert(pylonImage,ptrGrabResult);
            cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8UC1,(uint8_t*) pylonImage.GetBuffer());

            cam_img_r=cvImg;

            if(!cam_img_r.empty())
            {
                cv::imwrite("img_r.png",cam_img_r);
                ptrGrabResult.Release();
            }
        }
        cameras[1].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded())
        {
            cov.Convert(pylonImage,ptrGrabResult);
            cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8UC1,(uint8_t*) pylonImage.GetBuffer());

            cam_img_l=cvImg;

            if(!cam_img_l.empty())
            {
                cv::imwrite("img_l.png",cam_img_l);
                ptrGrabResult.Release();
            }
        }
        cameras.DestroyDevice();
        devices.empty();
    }
    catch (const GenericException& e)
    {
        cerr << "An exception occurred." << endl
            << e.GetDescription() << endl;
    }
}

void camera_::camera_set_parameter()
{
    CBaslerUsbInstantCameraArray cameras;
    CTlFactory& tlFactory = CTlFactory::GetInstance();
    DeviceInfoList_t lstDevices;
    int i = 0;

    // Find and configure camera resources
    if ( tlFactory.EnumerateDevices(lstDevices) > 0 )
    {
        //cerr << get_time_string() << " Found " << lstDevices.size() << " camera" << ((lstDevices.size() > 1)? "s" : "") << endl;
        cameras.Initialize(lstDevices.size());

        DeviceInfoList_t::const_iterator it;
        it = lstDevices.begin();
        for ( it = lstDevices.begin(); it != lstDevices.end(); ++it, ++i )
        {
            try
            {
                cameras[i].Attach(tlFactory.CreateDevice(*it));
                cameras[i].Open();
                cameras[i].ExposureTime.SetValue(set_exposure_time);
                cameras[i].Width.SetValue(set_width);
                cameras[i].Height.SetValue(set_height);
                //cameras[i].OffsetX.SetValue(set_offset_x);
                //cameras[i].OffsetY.SetValue(set_offset_y);
                cameras[i].AcquisitionFrameRate.SetValue(set_camera_fps);
                cameras[i].Close();
            }
            catch (const GenericException &e)
            {
                cerr << "An exception occurred." << endl
                    << e.GetDescription() << endl;
            }
        }
    }
    cameras.DestroyDevice();
}

void camera_::camera_read_parameter()
{
    CBaslerUsbInstantCameraArray cameras;
    CTlFactory& tlFactory = CTlFactory::GetInstance();
    DeviceInfoList_t lstDevices;

    if ( tlFactory.EnumerateDevices(lstDevices) > 0 )
    {
        cameras.Initialize(lstDevices.size());

        DeviceInfoList_t::const_iterator it;
        it = lstDevices.begin();

        try
        {
            cameras[0].Attach(tlFactory.CreateDevice(*it));
            cameras[0].Open();
            read_exposure_time=cameras[0].ExposureTime.GetValue();
            read_width=cameras[0].Width.GetValue();
            read_height=cameras[0].Height.GetValue();
            read_offset_x=cameras[0].OffsetX.GetValue();
            read_offset_y=cameras[0].OffsetY.GetValue();
            read_camera_fps=cameras[0].AcquisitionFrameRate.GetValue();
            cameras[0].Close();
        }
        catch (const GenericException &e)
        {
            cerr << "An exception occurred." << endl
                << e.GetDescription() << endl;
        }
    }
    cameras.DestroyDevice();
}

#endif

void camera_::calculate_relationship(std::vector<cv::Mat> external_Schwester,std::vector<cv::Mat> external_Bruder) // 解线性方程组
{
    for(size_t i=0;i<external_Bruder.size();i++)
    {
        for(size_t j=0;j<external_Schwester.size();j++)
        {
            Schwinge=external_Bruder[i]*external_Schwester[j];
        }
    }
}

void camera_::calculate_relationship() //计算重投影误差来确定使用那个矩阵
{
    size_t i,j,k;
    cv::Mat rm=cv::Mat(3,3,CV_64FC1,cv::Scalar::all(0));
    cv::Mat tm=cv::Mat(3,1,CV_64FC1,cv::Scalar::all(0));
    cv::Mat t1=cv::Mat(3,1,CV_64FC1,cv::Scalar::all(0));
    cv::Mat t2=cv::Mat(3,1,CV_64FC1,cv::Scalar::all(0));
    cv::Mat rs=cv::Mat(4,4,CV_64FC1,cv::Scalar::all(0));
    if(external_Bruder.size()==external_Schwester.size())
    {
        for(i=0;i<external_Bruder.size();i++)
        {
            rm=Bruder_rotation_matrix[i]*Schwester_rotation_matrix[i].t();

            for(j=0;j<3;j++)
            {
                for(k=0;k<3;k++)
                {
                    rs.at<double>(j,k)=rm.at<double>(j,k);
                }
            }

            t1.at<double>(0,0)=external_Bruder[i].at<double>(0,3);
            t1.at<double>(1,0)=external_Bruder[i].at<double>(1,3);
            t1.at<double>(2,0)=external_Bruder[i].at<double>(2,3);
            t2.at<double>(0,0)=external_Schwester[i].at<double>(0,3);
            t2.at<double>(1,0)=external_Schwester[i].at<double>(1,3);
            t2.at<double>(2,0)=external_Schwester[i].at<double>(2,3);
            tm=t1-rm*t2;

            rs.at<double>(0,3)=tm.at<double>(0,0);
            rs.at<double>(1,3)=tm.at<double>(1,0);
            rs.at<double>(2,3)=tm.at<double>(2,0);
            rs.at<double>(3,3)=1;
            std::cout<<"rs "<<i<<std::endl;
            std::cout<<rs<<std::endl;
        }
    }
    else
    {
        printf("size is not equal!!!\n");
    }
}

#ifdef use_opencv_camera //use the opencv control the usb camera

void camera_::img_grab_opencv()
{
   // VideoCapture capture(0); //0默认—摄像机，若用笔记本相机;1 --是USB有限，无USB就 网络，若用笔记本；-1选择多个相机界面
    cv::VideoCapture *camera_point=new cv::VideoCapture;
    camera_point=&opencv_camera;
    camera_point->open(Bruder_camera_ID);
    camera_point->grab();
    camera_point->retrieve(Bruder_img,1);
    if (!Bruder_img.empty())
    {
        cv::imwrite("img_l.png",Bruder_img);
    }
    else
    {
        std::cout << "can not "<<std::endl;//最后一帧的处理
        waitKey(30);
    }

    //camera_point->open(Schwester_camera_ID);
    camera_point->grab();
    camera_point->set(CAP_PROP_BRIGHTNESS,10);
    camera_point->set(CAP_PROP_FRAME_WIDTH,200);
    camera_point->retrieve(Schwester_img,1);
    if (!Schwester_img.empty())
    {
        cv::imwrite("img_r.png",Schwester_img);
    }
    else
    {
        std::cout << "can not "<<std::endl;//最后一帧的处理
        waitKey(30);
    }

    camera_point=NULL;
    delete camera_point;
}

void camera_::camera_set_parameter_opencv()
{
    cv::VideoCapture *camera_point=new cv::VideoCapture;
    camera_point=&opencv_camera;
    camera_point->open(0);
    camera_point->set(CAP_PROP_FRAME_WIDTH,set_width);
    //opencv_camera_Schwester.set(CAP_PROP_FRAME_WIDTH,set_width);
    camera_point->set(CAP_PROP_FRAME_HEIGHT,set_height);
    //opencv_camera_Schwester.set(CAP_PROP_FRAME_HEIGHT,set_height);
    camera_point->set(CAP_PROP_EXPOSURE,set_exposure_time);
    //opencv_camera_Schwester.set(CAP_PROP_EXPOSURE,set_exposure_time);
    camera_point=NULL;
    delete camera_point;
}

void camera_::camera_read_parameter_opencv()
{
    cv::VideoCapture *camera_point=new cv::VideoCapture;

    if(first_read_flag)
    {
        camera_point=&opencv_camera;
        camera_point->open(0);
        read_camera_fps=camera_point->get(CAP_PROP_FPS);
        read_width=camera_point->get(CAP_PROP_FRAME_WIDTH);
        read_height=camera_point->get(CAP_PROP_FRAME_HEIGHT);
        read_exposure_time=camera_point->get(CAP_PROP_EXPOSURE);
        max_width=read_width;
        max_height=read_height;
        first_read_flag=0;
    }
    else
    {
        read_camera_fps=set_camera_fps;
        read_offset_x=set_offset_x;
        read_offset_y=set_offset_y;
        read_exposure_time=set_exposure_time;
        read_width=set_width;
        read_height=set_height;
    }

    camera_point=NULL;
    delete camera_point;
}

void camera_::img_offset(Mat &input,Mat &output)
{
    int x_end=set_offset_x+set_width;
    int y_end=set_offset_y+set_height;
    if(x_end>max_width||y_end>max_height)
    {
        printf("the camera can not reach the region!!!\n");
    }
    else
    {
        output=input(Rect(set_offset_x,set_offset_y,set_width,set_height));
    }
}

#endif
