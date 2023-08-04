# buendia

# 相机控制

class basler_camera 用于basler相机控制，配置pylon7.2相机库

1 设置相机参数，包括分辨率，曝光时间，对类中的参数修改
void camera_set_parameter()

2 读取相机参数，结果保存在类的参数中
void camera_read_parameter()

3 控制相机采集灰度图片一次，直接保存图片至本地
void camera_grab()

4 控制相机采集彩色图片一次，直接保存图片至本地
void camera_rgb()

5 控制相机采集灰度图片一次，返回采集结果
cv::Mat camera_grab_return()

6 控制相机采集彩色图片一次，返回采集结果
cv::Mat camera_rgb_return()

# 结构光控制


# 振镜控制

class galvo_control 

1 控制两个振镜旋转一个固定角度
int galvo_rotate(float angle_ein,float angle_zwei)

2 读取两个振镜的角度
void galvo_read()

# modbus 通讯



# ethercat 通讯


# socket 通讯

class socket_

tcp 服务器
socket_server
socket_server_initial

tcp 客户端
socket_client
socket_client_initial

tcp 固定端口监听


