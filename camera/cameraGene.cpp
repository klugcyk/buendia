/*
    author:klug
    献给我的心上人等待天使的妹妹
    start:221129
    last:230628
*/

#include "camera/cameraGene.hpp"

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

void cameraGene::cameraCalibrate(std::vector<cv::Mat> img_vector,cv::Mat &camera_matrix,cv::Mat &dis)
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
        rms=calibrateCamera(object_points_seq,image_points_seq,image_size,camera_matrix,dis,rvecsMat,tvecsMat,cv::CALIB_FIX_K3+cv::CALIB_ZERO_TANGENT_DIST);
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

