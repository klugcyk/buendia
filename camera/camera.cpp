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
#ifdef camera_print_msg_info
            cout<<"Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
#endif
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
#ifdef camera_print_error_info
        cerr << "An exception occurred." << endl
            << e.GetDescription() << endl;
#endif
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
#ifdef camera_print_error_info
                cerr << "An exception occurred." << endl
                    << e.GetDescription() << endl;
#endif
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
#ifdef camera_print_error_info
            cerr << "An exception occurred." << endl
                << e.GetDescription() << endl;
#endif
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
