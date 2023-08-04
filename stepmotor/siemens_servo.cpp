/*
    文件等级：密一
    author:klug
    献给我的心上人等待天使的妹妹
    start:221129
    last:230310
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

static const size_t c_maxCamerasToUse =2;
cv::Mat camera_grab_img;

#ifdef use_pylon_camera

camera_::camera_()
{
    printf("construct tha camera class...\n");
    PylonInitialize();
}

camera_::~camera_()
{
    PylonTerminate();
}

void camera_::camera_grab()
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

        cameras[0].GrabOne(5000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded())
        {
            cov.Convert(pylonImage,ptrGrabResult);
            cv::Mat cvImg(ptrGrabResult->GetHeight(),ptrGrabResult->GetWidth(),CV_8UC1,(uint8_t*) pylonImage.GetBuffer());

            cam_img=cvImg;
            printf("camera_grab_img.channels:=%d\n",camera_grab_img.channels());

            if(!cam_img.empty())
            {
                cv::imwrite("cam_img.png",cam_img);
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

#endif

#ifdef use_laserscan_camera

camera_laserscan::camera_laserscan()
{
#ifdef camera_print_msg_info
    printf("start the camera laserscan...\n");
#endif

}

camera_laserscan::~camera_laserscan()
{
#ifdef camera_print_msg_info
    printf("end the camera laserscan...\n");
#endif

}

#endif
