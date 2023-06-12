/*
    文件等级：密一
    author:klug
    献给我的心上人等待天使的妹妹
    start:230426
    last:230525
*/

#ifndef least_sqaure_HPP
#define least_sqaure_HPP

#include "geometry.hpp"
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include <Eigen/Core>
#include <Eigen/Dense>

#define least_sqaure_msg_info
#define least_sqaure_data_info
#define least_sqaure_error_info

namespace math_leastsqaure
{

math_geometry::geo_plane_param plane_calculate(std::vector<Eigen::Vector3d> points);
math_geometry::geo_plane_param plane_calculate4(std::vector<Eigen::Vector3d> points);
math_geometry::geo_line_param line_calculate_2d(std::vector<Eigen::Vector2d> points);
math_geometry::geo_line_param line_calculate_3d(std::vector<Eigen::Vector3d> points);
math_geometry::geo_line_param line_calculate_2d(std::vector<cv::Point2f> points);
math_geometry::geo_line_param line_calculate_3d(std::vector<cv::Point2f> points);
math_geometry::geo_line_param line_calculate_2d(std::vector<cv::Point2f> points,int point_use);

};

#endif // least_sqaure_HPP
