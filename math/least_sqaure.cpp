/*
    文件等级：密一
    author:klug
    献给特瓦尔多夫斯基
    start:230426
    last:230525
*/

#include "least_sqaure.hpp"
#include <Eigen/Core>
#include <Eigen/Dense>

namespace math_leastsqaure
{

/*
    最小二乘法拟合三维直线
    @points:点的集合
    @res_line:返回值，拟合结果直线
*/
math_geometry::geo_line_param line_calculate_3d(std::vector<Eigen::Vector2d> points)
{
    math_geometry::geo_line_param res_line;

    // 构造最小二乘方程

    // 求解最小二乘方程

    return res_line;
}

/*
    最小二乘法拟合二维直线
    @points:点的集合
    @res_line:返回值，拟合结果直线
*/
math_geometry::geo_line_param line_calculate_2d(std::vector<Eigen::Vector2d> points)
{
    math_geometry::geo_line_param res_line;

    // 构造最小二乘方程
    double a11=0;
    double a12=0;
    double a21=0;
    double a22=0;
    double b1=0;
    double b2=0;

    for(size_t point_cnt=0;point_cnt<points.size();point_cnt++)
    {
        a11+=points[point_cnt](0)*points[point_cnt](0);
        a12+=points[point_cnt](0);
        b1+=points[point_cnt](0)*points[point_cnt](1);
        b2+=points[point_cnt](1);
    }

    a21=a12;
    a22=points.size()+1;

    // 求解最小二乘方程
    Eigen::Matrix2d equation_link(2,2);
    equation_link<<a11,a12,a21,a22;
    Eigen::Vector2d equation_recht(2,1);
    equation_recht<<b1,b2;

    Eigen::Vector2d res=equation_link.llt().solve(equation_recht);

    res_line.k2=res(0);
    res_line.b2=res(1);

    return res_line;
}

/*
    最小二乘法拟合二维直线
    @points:点的集合
    @res_line:返回值，拟合结果直线
*/
math_geometry::geo_line_param line_calculate_2d(std::vector<cv::Point2f> points)
{
    math_geometry::geo_line_param res_line;

    // 构造最小二乘方程
    double a11=0;
    double a12=0;
    double a21=0;
    double a22=0;
    double b1=0;
    double b2=0;

    for(size_t point_cnt=0;point_cnt<points.size();point_cnt++)
    {
        a11+=points[point_cnt].x*points[point_cnt].x;
        a12+=points[point_cnt].x;
        a21+=points[point_cnt].x;
        a22+=1;
        b1+=points[point_cnt].x*points[point_cnt].y;
        b2+=points[point_cnt].y;
    }

    res_line.k2=(a22*b1-a12*b2)/(a11*a22-a12*a21);
    res_line.b2=(b1-res_line.k2*a11)/a12;

    return res_line;
}

/*
    最小二乘法拟合二维直线
    @points:点的集合
    @point_use:拟合时使用点的数量
    @res_line:返回值，拟合结果直线
*/
math_geometry::geo_line_param line_calculate_2d(std::vector<cv::Point2f> points,int point_use)
{
    math_geometry::geo_line_param res_line;

    // 构造最小二乘方程
    double a11=0;
    double a12=0;
    double a21=0;
    double a22=0;
    double b1=0;
    double b2=0;

    int pu=0;

    if(point_use>points.size())
    {
        pu=points.size();
    }
    else
    {
        pu=point_use;
    }

    for(size_t point_cnt=0;point_cnt<pu;point_cnt++)
    {
        a11+=points[point_cnt].x*points[point_cnt].x;
        a12+=points[point_cnt].x;
        a21+=points[point_cnt].x;
        a22+=1;
        b1+=points[point_cnt].x*points[point_cnt].y;
        b2+=points[point_cnt].y;
    }

    res_line.k2=(a22*b1-a12*b2)/(a11*a22-a12*a21);
    res_line.b2=(b1-res_line.k2*a11)/a12;

    return res_line;
}

/*
    最小二乘法拟合平面
    @points:点的集合
    @res_plane:返回值，拟合平面结果
*/
math_geometry::geo_plane_param plane_calculate(std::vector<Eigen::Vector3d> points)
{
    math_geometry::geo_plane_param res_plane;

    //构造最小二乘法方程
    double a11=0,a12=0,a13=0;
    double a21=0,a22=0,a23=0;
    double a31=0,a32=0,a33=0;
    double d1=0,d2=0,d3=0;

    for(size_t point_cnt=0;point_cnt<points.size();point_cnt++)
    {
        a11+=points[point_cnt](0)*points[point_cnt](0);
        a12+=points[point_cnt](0)*points[point_cnt](1);
        a13+=points[point_cnt](0);

        a21+=points[point_cnt](0)*points[point_cnt](1);
        a22+=points[point_cnt](1)*points[point_cnt](1);
        a23+=points[point_cnt](1);

        a31+=points[point_cnt](0);
        a32+=points[point_cnt](1);
        a33+=1;

        d1+=points[point_cnt](0)*points[point_cnt](2);
        d2+=points[point_cnt](1)*points[point_cnt](2);
        d3+=points[point_cnt](2);
    }

    //求解最小二乘法方程
    Eigen::Matrix3d equation_link(3,3);
    equation_link<<a11,a12,a13,a21,a22,a23,a31,a32,a33;
    Eigen::Vector3d equation_recht(3,1);
    equation_recht<<d1,d2,d3;

    Eigen::Vector3d res=equation_link.llt().solve(equation_recht);

    res_plane.A=res(0);
    res_plane.B=res(1);
    res_plane.C=-1;
    res_plane.D=res(2);

    return res_plane;
}

/*
    最小二乘法拟合平面
    @points:点的集合
    @res_plane:返回值，拟合平面结果
*/
math_geometry::geo_plane_param plane_calculate4(std::vector<Eigen::Vector3d> points)
{
    math_geometry::geo_plane_param res_plane;

    //构造最小二乘法方程
    long l11=0,l12=0,l13=0,l14=0;
    long l21=0,l22=0,l23=0,l24=0;
    long l31=0,l32=0,l33=0,l34=0;
    long l41=0,l42=0,l43=0,l44=0;
    long r1=0,r2=0,r3=0,r4=0;

    for(size_t point_cnt=0;point_cnt<points.size();point_cnt++)
    {
        l11+=2*(points[point_cnt].x()*points[point_cnt].x());
        l12+=2*(points[point_cnt].x()*points[point_cnt].y());
        l13+=2*(points[point_cnt].x()*points[point_cnt].z());
        l14+=2*points[point_cnt].x();

        l21=2*(points[point_cnt].y()*points[point_cnt].x());
        l22=2*(points[point_cnt].y()*points[point_cnt].y());
        l23=2*(points[point_cnt].y()*points[point_cnt].z());
        l24=2*points[point_cnt].y();

        l31+=2*(points[point_cnt].z()*points[point_cnt].x());
        l32+=2*(points[point_cnt].z()*points[point_cnt].y());
        l33+=2*(points[point_cnt].z()*points[point_cnt].z());
        l34+=2*points[point_cnt].z();

        l41+=2*points[point_cnt].x();
        l42+=2*points[point_cnt].y();
        l43+=2*points[point_cnt].z();
        l44+=2;
    }

    //求解最小二乘法方程
    Eigen::Matrix4d equation_link(4,4);
    equation_link<<l11,l12,l13,l14,l21,l22,l23,l24,l31,l32,l33,l34,l41,l42,l43,l44;;
    Eigen::Vector4d equation_recht(4,1);
    equation_recht<<r1,r2,r3,r4;

    Eigen::Vector4d res=equation_link.colPivHouseholderQr().solve(equation_recht);

    res_plane.A=res(0);
    res_plane.B=res(1);
    res_plane.C=res(2);
    res_plane.D=res(3);

    return res_plane;
}

};
