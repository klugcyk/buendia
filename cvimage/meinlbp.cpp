/*
    author:klug
    献给我的心上人等待天使的妹妹
    start:230625
    last:230625
*/

#include "cvimage/meinlbp.hpp"

namespace meincv
{

/*
    LBP特征生成图片,gray
    @src_img:输入图像
    @res_img:LBP结果图像
*/
void meinlbp(cv::Mat src_img,cv::Mat &res_img)
{
    cv::Mat img=src_img;
    if(src_img.channels()!=1)
    {
        cv::cvtColor(src_img,img,cv::COLOR_BGR2GRAY);
    }

    for(size_t row=1;row<src_img.rows-1;row++)
    {
        for(size_t col=1;col<src_img.cols-1;col++)
        {
            uchar zenturm_gray=img.at<uchar>(row,col);
            uchar res_gray=0;
            res_gray|=(zenturm_gray>img.at<uchar>(row-1,col-1))<<7;
            res_gray|=(zenturm_gray>img.at<uchar>(row-1,col))<<6;
            res_gray|=(zenturm_gray>img.at<uchar>(row-1,col+1))<<5;
            res_gray|=(zenturm_gray>img.at<uchar>(row,col+1))<<4;
            res_gray|=(zenturm_gray>img.at<uchar>(row+1,col+1))<<3;
            res_gray|=(zenturm_gray>img.at<uchar>(row+1,col))<<2;
            res_gray|=(zenturm_gray>img.at<uchar>(row+1,col-1))<<1;
            res_gray|=(zenturm_gray>img.at<uchar>(row,col-1));
            /*if(zenturm_gray>src_img.at<uchar>(row-1,col-1))
            {
                res_gray+=128;
            }
            if(zenturm_gray>src_img.at<uchar>(row-1,col))
            {
                res_gray+=64;
            }
            if(zenturm_gray>src_img.at<uchar>(row-1,col+1))
            {
                res_gray+=32;
            }
            if(zenturm_gray>src_img.at<uchar>(row,col+1))
            {
                res_gray+=16;
            }
            if(zenturm_gray>src_img.at<uchar>(row+1,col+1))
            {
                res_gray+=8;
            }
            if(zenturm_gray>src_img.at<uchar>(row+1,col))
            {
                res_gray+=4;
            }
            if(zenturm_gray>src_img.at<uchar>(row+1,col-1))
            {
                res_gray+=2;
            }
            if(zenturm_gray>src_img.at<uchar>(row,col-1))
            {
                res_gray+=1;
            }*/
            res_img.at<uchar>(row,col)=res_gray;
        }
    }
}

/*
    LBP特征生成图片
    @src_img:输入图像
    @res_img:LBP结果图像
    @CircleRadius:LBP作用域半径
*/
void meinlbp(cv::Mat src_img,cv::Mat &res_img,float CircleRadius)
{

}

};
