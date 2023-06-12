/*
    文件等级：密一
    author:klug
    献给杜尔西内娅德尔托博索
    start:221123
    last:230215
*/

#ifndef _modbus_zenturm
#define _modbus_zenturm

#include <memory>
#include <chrono>
#include "modbus.h"
#include <mutex>
#include <functional>     // std::ref
#include <thread>         // std::thread
#include <future>         // std::promise, std::future
#include <exception>      // std::exception, std::current_exception
#include "commonsource.hpp"

#define modbus_port 1496
#define modbus_server_ip "0.0.0.0" //for modbus client
#define modbus_server_port 1496 //for modbus clinet
#define modbus_mapping_1 0
#define modbus_mapping_2 0
#define modbus_mapping_3 0x2000
#define modbus_mapping_4 0x2000

// robot modbus address
#define modbus_robot_read_x_h 0x000
#define modbus_robot_read_x_l 0x001
#define modbus_robot_read_y_h 0x002
#define modbus_robot_read_y_l 0x003
#define modbus_robot_read_z_h 0x004
#define modbus_robot_read_z_l 0x005
#define modbus_robot_read_rx_h 0x006
#define modbus_robot_read_rx_l 0x007
#define modbus_robot_read_ry_h 0x008
#define modbus_robot_read_ry_l 0x009
#define modbus_robot_read_rz_h 0x00a
#define modbus_robot_read_rz_l 0x00b
#define modbus_robot_speed_h 0x00c
#define modbus_robot_speed_l 0x00d
#define modbus_robot_state 0x00e
#define modbus_robot_hour 0x00f
#define modbus_robot_minute 0x010
#define modbus_robot_second 0x011
#define modbus_robot_msecond 0x012

#define modbus_robot_choose 0x100
#define modbus_robot_tcp 0x101
#define modbus_robot_move_func 0x102
#define modbus_robot_move_weld 0x103
#define modbus_robot_move_speed_h 0x104
#define modbus_robot_move_speed_l 0x105
#define modbus_robot_set_x_h 0x106
#define modbus_robot_set_x_l 0x107
#define modbus_robot_set_y_h 0x108
#define modbus_robot_set_y_l 0x109
#define modbus_robot_set_z_h 0x10a
#define modbus_robot_set_z_l 0x10b
#define modbus_robot_set_rx_h 0x10c
#define modbus_robot_set_rx_l 0x10d
#define modbus_robot_set_ry_h 0x10e
#define modbus_robot_set_ry_l 0x10f
#define modbus_robot_set_rz_h 0x110
#define modbus_robot_set_rz_l 0x111

#define modbus_weld_electric_h 0x200
#define modbus_weld_electric_l 0x201
#define modbus_weld_electric_type 0x202

#define modbus_weld_machine_choose 0x250
#define modbus_weld_machine_ip_1 0x251
#define modbus_weld_machine_ip_2 0x252
#define modbus_weld_machine_ip_3 0x253
#define modbus_weld_machine_ip_4 0x254

#define modbus_robot_ip_1 0x300
#define modbus_robot_ip_2 0x301
#define modbus_robot_ip_3 0x302
#define modbus_robot_ip_4 0x303

#define modbus_robot_stop_model 0x400

#define modbus_fifo_array 0x4aa

#define modbus_print_msg_info
#define modbus_print_error_msg_info
//#define modbus_print_data_info

#define fifo_position_threshold 0.01
#define fifo_posture_threshold 0.01
#define fifo_max 2000

class modbus_zenturm:public commonsource
{
public:
    modbus_zenturm();
    ~modbus_zenturm();
    int robot_choose_=0; //receive from up-machine send to the robot control node

    void _modbus(int port);
    void modbus_initial(int port);
    void modbus_data_communication();
    void modbus_client(std::string server_ip,int server_port);
    void modbus_robot_test();
    void fifo_write();

    size_t count_;

    std::thread modbus_test_thread;
    std::thread modbus_server_thread;
    std::thread modbus_client_thread;
    std::thread data_comm_thread;
    std::mutex modbus_mtx;

    int ip_1=6174;
    int ip_2=6174;
    int ip_3=6174;
    int ip_4=6174;

    int robot_tcp_choose=6174;
    int robot_software_stop=6174;
    int move_function=6174;
    //int robot_choose=6174;

    float movel_speed=6174;

    float read_position_x=0.0; //receive from the robot control node send to up-machine
    float read_position_y=0.0;
    float read_position_z=0.0;
    float read_posture_rx=0.0;
    float read_posture_ry=0.0;
    float read_posture_rz=0.0;

    float set_position_x=0.0; //receive from up_machine send to robot control node
    float set_position_y=0.0;
    float set_position_z=0.0;
    float set_posture_rx=0.0;
    float set_posture_ry=0.0;
    float set_posture_rz=0.0;

    float robot_hour=0;
    float robot_minute=0;
    float robot_second=0;
    float robot_msecond=0;

    float set_move_speed=0;

    int robot_state=0;

    bool update_flag=1;

};

#endif
