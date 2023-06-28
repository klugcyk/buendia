/*
    文件等级：密一
    author:klug
    献给我的心上人等待天使的妹妹
    start:221129
    last:230625
*/

#include "camera.hpp"
#include "source.hpp"
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
#include "ConfigurationEventPrinter.h"
#include "CameraEventPrinter.h"

//std::mutex continue_lock;

using namespace Pylon;
using namespace Basler_UniversalCameraParams;
using namespace GenApi;
using namespace Basler_UsbCameraParams;

#endif

using namespace std;
using namespace cv;

static const size_t c_maxCamerasToUse =2;

#ifdef use_pylon_camera

basler_camera::basler_camera()
{
#ifdef camera_print_msg_info
    printf("open the camera class...\n");
#endif
    PylonInitialize();
#if camera_continue==1
    if(camera_continue_switch)
    {
        continue_img_thread=std::thread(&basler_camera::camera_video_offnen,this);
    }

#endif
}

basler_camera::basler_camera(int mod)
{
#ifdef camera_print_msg_info
    printf("open the camera class...\n");
#endif
    PylonInitialize();
    if(mod==1)
    {
        continue_img_thread=std::thread(&basler_camera::camera_video_offnen,this);
    }
    grab_mod=mod;
}

basler_camera::~basler_camera()
{
#ifdef camera_print_msg_info
    printf("close the camera class...\n");
#endif
#if camera_continue==1
    if(camera_continue_switch||grab_mod==1)
    {
        continue_img_thread.join();
    }
#endif
    PylonTerminate();
}

void basler_camera::camera_grab_rgb()
{
    try
    {
        CTlFactory& tlFactory = CTlFactory::GetInstance();

        DeviceInfoList_t devices;
        if (tlFactory.EnumerateDevices( devices ) == 0)
        {
            throw RUNTIME_EXCEPTION("No camera present.");
        }

        CInstantCameraArray cameras(min(devices.size(),c_maxCamerasToUse));
        for (size_t i = 0; i < cameras.GetSize(); ++i)
        {
            cameras[i].Attach( tlFactory.CreateDevice(devices[i]));
            cout<<"Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
        }

        CGrabResultPtr ptrGrabResult;
        CPylonImage pylonImage;
        CImageFormatConverter cov;
        cov.OutputPixelFormat = PixelType_BGR8packed;

        cameras[0].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded())
        {
            cov.Convert(pylonImage,ptrGrabResult);
            cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8UC3,(uint8_t*) pylonImage.GetBuffer());

            cam_img=cvImg;
#ifdef camera_print_data_info
            printf("camera_grab_img.channels:=%d\n",cvImg.channels());
#endif
            if(!cam_img.empty())
            {
                cv::imwrite("/home/klug/img/construct/cam_img.png",cam_img);
                ptrGrabResult.Release();
            }
            cam_width=cam_img.cols;
            cam_height=cam_img.rows;
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

cv::Mat basler_camera::camera_grab_rgb_return()
{
    CTlFactory& tlFactory = CTlFactory::GetInstance();

    DeviceInfoList_t devices;
    if (tlFactory.EnumerateDevices( devices ) == 0)
    {
        throw RUNTIME_EXCEPTION("No camera present.");
    }

    CInstantCameraArray cameras(min(devices.size(),c_maxCamerasToUse));
    for (size_t i = 0; i < cameras.GetSize(); ++i)
    {
        cameras[i].Attach( tlFactory.CreateDevice(devices[i]));
        cout<<"Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
    }

    CGrabResultPtr ptrGrabResult;
    CPylonImage pylonImage;
    CImageFormatConverter cov;
    cov.OutputPixelFormat = PixelType_BGR8packed;

    cameras[0].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
    if (ptrGrabResult->GrabSucceeded())
    {
        cov.Convert(pylonImage,ptrGrabResult);
        cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8UC3,(uint8_t*) pylonImage.GetBuffer());

        cam_img=cvImg;
#ifdef camera_print_data_info
        printf("camera_grab_img.channels:=%d\n",cvImg.channels());
#endif
        if(!cam_img.empty())
        {
#ifdef camera_save
            cv::imwrite("/home/klug/img/construct/cam_img.png",cam_img);
#endif
            ptrGrabResult.Release();
        }
    }

    cameras.DestroyDevice();
    devices.empty();

    return cam_img;
}

cv::Mat basler_camera::camera_grab_gray_return()
{

    CTlFactory& tlFactory = CTlFactory::GetInstance();

    DeviceInfoList_t devices;
    if (tlFactory.EnumerateDevices( devices ) == 0)
    {
        throw RUNTIME_EXCEPTION("No camera present.");
    }

    CInstantCameraArray cameras(min(devices.size(),c_maxCamerasToUse));
    for (size_t i = 0; i < cameras.GetSize(); ++i)
    {
        cameras[i].Attach( tlFactory.CreateDevice(devices[i]));
        cout<<"Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
    }

    CGrabResultPtr ptrGrabResult;
    CPylonImage pylonImage;
    CImageFormatConverter cov;

    cameras[0].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
    if (ptrGrabResult->GrabSucceeded())
    {
        cov.Convert(pylonImage,ptrGrabResult);
        cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8U,(uint8_t*) pylonImage.GetBuffer());

        cam_img=cvImg;
#ifdef camera_print_data_info
        printf("camera_grab_img.channels:=%d\n",cvImg.channels());
#endif
        if(!cam_img.empty())
        {
#ifdef camera_save
            cv::imwrite("/home/klug/img/construct/cam_img.png",cam_img);
#endif
            ptrGrabResult.Release();
        }
    }

    cameras.DestroyDevice();
    devices.empty();
    return cam_img;
}

void basler_camera::camera_grab_gray()
{
    try
    {
        CTlFactory& tlFactory = CTlFactory::GetInstance();

        DeviceInfoList_t devices;
        if (tlFactory.EnumerateDevices( devices ) == 0)
        {
            throw RUNTIME_EXCEPTION("No camera present.");
        }

        CInstantCameraArray cameras(min(devices.size(),c_maxCamerasToUse));
        for (size_t i = 0; i < cameras.GetSize(); ++i)
        {
            cameras[i].Attach( tlFactory.CreateDevice(devices[i]));
            cout<<"Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
        }

        CGrabResultPtr ptrGrabResult;
        CPylonImage pylonImage;
        CImageFormatConverter cov;

        cameras[0].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded())
        {
            cov.Convert(pylonImage,ptrGrabResult);
            cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8U,(uint8_t*) pylonImage.GetBuffer());

            cam_img=cvImg;
#ifdef camera_print_data_info
            printf("camera_grab_img.channels:=%d\n",cvImg.channels());
#endif
            if(!cam_img.empty())
            {
                cv::imwrite("/home/klug/img/construct/cam_img.png",cam_img);
                ptrGrabResult.Release();
            }
            cam_width=cam_img.cols;
            cam_height=cam_img.rows;
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

void basler_camera::camera_grab_zwei()
{
    try
    {
        CTlFactory& tlFactory = CTlFactory::GetInstance();

        DeviceInfoList_t devices;
        if (tlFactory.EnumerateDevices( devices ) == 0)
        {
            throw RUNTIME_EXCEPTION("No camera present.");
        }

        CInstantCameraArray cameras(min(devices.size(),c_maxCamerasToUse));
        for (size_t i = 0; i < cameras.GetSize(); ++i)
        {
            cameras[i].Attach( tlFactory.CreateDevice(devices[i]));
            cout<<"Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
        }

        CGrabResultPtr ptrGrabResult;
        CPylonImage pylonImage;
        CImageFormatConverter cov;

        cameras[0].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded())
        {
            cov.Convert(pylonImage,ptrGrabResult);
            cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8U,(uint8_t*) pylonImage.GetBuffer());

            cam_img=cvImg;
#ifdef camera_print_data_info
            printf("camera_grab_img.channels:=%d\n",cvImg.channels());
#endif
            if(!cam_img.empty())
            {
                cv::imwrite("/home/klug/img/zwei_construct/cam_link.png",cam_img);
                ptrGrabResult.Release();
            }
        }

        cameras[1].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded())
        {
            cov.Convert(pylonImage,ptrGrabResult);
            cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8U,(uint8_t*) pylonImage.GetBuffer());

            cam_img=cvImg;
#ifdef camera_print_data_info
            printf("camera_grab_img.channels:=%d\n",cvImg.channels());
#endif
            if(!cam_img.empty())
            {
                cv::imwrite("/home/klug/img/zwei_construct/cam_richt.png",cam_img);
                ptrGrabResult.Release();
            }
            cam_width=cam_img.cols;
            cam_height=cam_img.rows;
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

void basler_camera::camera_set_parameter()
{
    CBaslerUsbInstantCameraArray cameras;
    CTlFactory& tlFactory = CTlFactory::GetInstance();
    DeviceInfoList_t lstDevices;
    int i = 0;

    // Find and configure camera resources
    if(tlFactory.EnumerateDevices(lstDevices)>0)
    {
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

void basler_camera::camera_read_parameter()
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
            //read_offset_x=cameras[0].OffsetX.GetValue();
            //read_offset_y=cameras[0].OffsetY.GetValue();
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

void basler_camera::camera_set_parameter_zwei()
{
    CBaslerUsbInstantCameraArray cameras;
    CTlFactory& tlFactory = CTlFactory::GetInstance();
    DeviceInfoList_t lstDevices;
    int i = 0;

    // Find and configure camera resources
    if(tlFactory.EnumerateDevices(lstDevices)>0)
    {
        cameras.Initialize(lstDevices.size());

        DeviceInfoList_t::const_iterator it;
        it = lstDevices.begin();

        try
        {
            cameras[0].Attach(tlFactory.CreateDevice(*it));
            cameras[0].Open();
            cameras[0].ExposureTime.SetValue(camera_exposure_time_link);
            //cameras[0].Width.SetValue(set_width);
            //cameras[0].Height.SetValue(set_height);
            //cameras[0].AcquisitionFrameRate.SetValue(set_camera_fps);
            cameras[0].Close();

            cameras[1].Attach(tlFactory.CreateDevice(*it));
            cameras[1].Open();
            cameras[1].ExposureTime.SetValue(camera_exposure_time_richt);
            //cameras[1].Width.SetValue(set_width);
            //cameras[1].Height.SetValue(set_height);
            //cameras[1].AcquisitionFrameRate.SetValue(set_camera_fps);
            cameras[1].Close();
        }
        catch (const GenericException &e)
        {
            cerr << "An exception occurred." << endl
                << e.GetDescription() << endl;
        }

    }
    cameras.DestroyDevice();
}

void basler_camera::camera_read_parameter_zwei()
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
            camera_exposure_time_link=cameras[0].ExposureTime.GetValue();
            read_width=cameras[0].Width.GetValue();
            read_height=cameras[0].Height.GetValue();
            read_camera_fps=cameras[0].AcquisitionFrameRate.GetValue();
            cameras[0].Close();

            cameras[1].Attach(tlFactory.CreateDevice(*it));
            cameras[1].Open();
            camera_exposure_time_richt=cameras[1].ExposureTime.GetValue();
            read_width=cameras[1].Width.GetValue();
            read_height=cameras[1].Height.GetValue();
            read_camera_fps=cameras[1].AcquisitionFrameRate.GetValue();
            cameras[1].Close();
        }
        catch (const GenericException &e)
        {
            cerr << "An exception occurred." << endl
                << e.GetDescription() << endl;
        }
    }
    cameras.DestroyDevice();
}   

void basler_camera::camera_video_offnen()
{
    try
    {
        CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice() );
        //camera.RegisterConfiguration();
        camera.RegisterConfiguration( new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete );

        camera.Open();
        if(camera.CanWaitForFrameTriggerReady())
        {
            camera.StartGrabbing(GrabStrategy_OneByOne,GrabLoop_ProvidedByUser);
            CGrabResultPtr ptrGrabResult;
            CImageFormatConverter cov;
            CPylonImage pylonImage;
            cov.OutputPixelFormat=PixelType_BGR8packed;

            while(camera.IsGrabbing())
            {

                camera.ExecuteSoftwareTrigger();
                camera.RetrieveResult(1000,ptrGrabResult,TimeoutHandling_ThrowException);
                if(ptrGrabResult->GrabSucceeded())
                {
                    cov.Convert(pylonImage,ptrGrabResult);
#ifdef camera_print_data_info
                    cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
                    cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
#endif
                    cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8UC3,(uint8_t*) pylonImage.GetBuffer());
                    if(!cvImg.empty())
                    {
                        std::unique_lock<std::mutex> cam_lock(continue_lock);
                        cam_img_continue=cvImg;
                        cam_lock.unlock();
                    }
                }
            }
        }
        camera.Close();
    }
    catch (const GenericException& e)
    {
        cerr << "An exception occurred." << endl
            << e.GetDescription() << endl;
    }
}

#endif

/*
    初始化标定参数
    @xSize:标定板尺寸X
    @ySize:标定板尺寸Y
    @xLength:标定板格子长度X
    @yLength:标定板格子长度Y
*/
cameraGene::cameraGene(int xSize,int ySize,float xLength,float yLength)
{
    board_size.height=xSize;
    board_size.width=ySize;
    square_size.height=xLength;
    square_size.width=yLength;
#ifdef camera_print_msg_info
    printf("initial camera gene operate...\n");
#endif

}

cameraGene::cameraGene()
{
#ifdef camera_print_msg_info
    printf("open camera gene operate...\n");
#endif

}

cameraGene::~cameraGene()
{
#ifdef camera_print_msg_info
    printf("end camera gene operate...\n");
#endif

}

void cameraGene::cameraCalibrate(std::vector<cv::Mat> img_vector)
{
    int image_count=0;
    cv::Size image_size;

    std::vector<cv::Point2f> image_points;
    std::vector<std::vector<cv::Point2f>> image_points_seq;

    for(size_t i=0;i<img_vector.size();i++)
    {
        image_count++;
#ifdef construct_cal_print_msg_info
        std::cout << "image_count=" << image_count << std::endl;
#endif
        cv::Mat imageInput = img_vector[i];
        if(image_count==1)
        {
            image_size.width = imageInput.cols;
            image_size.height = imageInput.rows;
        }

        bool bRes=findChessboardCorners(imageInput,board_size,image_points,0);

        if(bRes)
        {
            cv::Mat view_gray;
            cvtColor(imageInput,view_gray,cv::COLOR_RGB2GRAY);
            cv::cornerSubPix(view_gray,image_points,cv::Size(11,11),cv::Size(-1, -1),cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT,30,0.01));
            image_points_seq.push_back(image_points);
            drawChessboardCorners(view_gray,board_size,image_points,true);
#ifdef construct_cal_save_process
            write_path=write_img_path;
            write_path+="view_gray_";
            write_path+=std::to_string(i+1);
            write_path+=".png";
            cv::imwrite(write_path,view_gray);
#endif
        }
        else
        {
#ifdef construct_cal_print_error_info
            printf("img fail...\n");
#endif
        }
    }

    std::vector<std::vector<cv::Point3f>> object_points_seq;

    for(int t=0;t<image_count;t++)
    {
        std::vector<cv::Point3f> object_points;
        for (int i = 0; i < board_size.height; i++)
        {
            for (int j = 0; j < board_size.width; j++)
            {
                cv::Point3f realPoint;
                realPoint.x = i * square_size.width;
                realPoint.y = j * square_size.height;
                realPoint.z = 0;
                object_points.push_back(realPoint);
            }
        }
        object_points_seq.push_back(object_points);
    }

    double rms;
    if(object_points_seq.size()==image_points_seq.size())
    {
        rms=calibrateCamera(object_points_seq,image_points_seq,image_size,cameraMatrix,distCoeffs,rvecsMat,tvecsMat, cv::CALIB_FIX_K3+cv::CALIB_ZERO_TANGENT_DIST);
    }

#ifdef camera_print_data_info
    printf("rms:=%f\n",rms);
#endif

    rotation_matrix.clear();
    for(size_t i=0;i<rvecsMat.size();i++)
    {
        cv::Mat rm = cv::Mat(3,3,CV_64FC1,cv::Scalar::all(0));
        Rodrigues(rvecsMat[i],rm);
        rotation_matrix.push_back(rm);
    }

    extrinsic.clear();
    for(size_t i=0;i<rotation_matrix.size();i++)
    {
        cv::Mat tm=cv::Mat(4,4,CV_64FC1,cv::Scalar::all(0));
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
        extrinsic.push_back(tm);
    }

}
