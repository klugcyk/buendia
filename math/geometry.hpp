/*
    文件等级：密一
    author:klug
    献给不喜欢我的弗雷德里希冯海因洛特
    start:230426
    last:230605
*/

#ifndef geometry_HPP
#define geometry_HPP

#define geometry_print_msg_info
#define geometry_print_data_info
#define geometry_print_error_info

namespace math_geometry
{

struct geo_plane_param
{
    float A;
    float B;
    float C;
    float D;
};


struct geo_line_param
{
    float a; //三维直线
    float b;
    float c;
    float x0;
    float y0;
    float z0;

    double k1; //二维直线
    double b1;

    double k2; //二维直线
    double b2;
};

struct geo_plane_paramd
{
    double A;
    double B;
    double C;
    double D;
};


struct geo_line_paramd
{
    double a;
    double b;
    double c;
    double x0;
    double y0;
    double z0;

    double k1;
    double k2;
    double b1;
    double b2;
};

struct point3
{
    float x;
    float y;
    float z;
};

struct point2
{
    float x;
    float y;
};

};

#endif // least_sqaure_HPP
