/*
    author:klug
    献给我的心上人等待天使的妹妹
    start:230625
    last:230625
*/

#ifndef mein_lbp
#define mein_lbp

#include <opencv2/opencv.hpp>

namespace meincv
{

void meinlbp(cv::Mat src_img,cv::Mat &res_img);
void meinlbp(cv::Mat src_img,cv::Mat &res_img,float CircleRadius);

};

#endif
