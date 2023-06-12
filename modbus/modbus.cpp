/*
    文件等级：密一
    author:klug
    献给我的心上人等待天使的妹妹
    start:221123
    last:230215
*/

#include "commonsource.hpp"
#include "modbus.hpp"
#include "modbus.h"
#include <unistd.h>
#include <memory>
#include <set>
#include <memory>
#include <chrono>
#include <thread>
#include <iostream>

bool read_upmachine_flag=0;
std::deque<cv::Point3f> fifo_position;
std::deque<cv::Point3f> fifo_posture;
std::mutex fifo_lock;
std::string robot_ip;

modbus_zenturm::modbus_zenturm()
{
#ifdef modbus_print_msg_info
    printf("start the modbus Zenturm...\n");
#endif
    //modbus_client_thread=std::thread(&modbus_zenturm::modbus_client,this,modbus_server_ip,modbus_server_port);
    modbus_server_thread=std::thread(&modbus_zenturm::_modbus,this,modbus_port); //circle
    data_comm_thread=std::thread(&modbus_zenturm::modbus_data_communication,this);
    modbus_test_thread=std::thread(&modbus_zenturm::modbus_robot_test,this);
}

modbus_zenturm::~modbus_zenturm()
{
    modbus_test_thread.join();
    //modbus_client_thread.join();
    modbus_server_thread.join();
    data_comm_thread.join();
}

void modbus_zenturm::modbus_robot_test()
{
    while(1)
    {
        while(1)
        {
            if(abs(read_position_x)>0)
            {
                set_position_x+=0.01;//from up machine
                set_position_y+=0.01;
                set_position_z+=0.01;
                set_posture_rx+=0.01;
                set_posture_ry+=0.01;
                set_posture_rz+=0.01;
                read_upmachine_flag=1;
            }

            usleep(10000);
        }
    }
}

void modbus_zenturm::_modbus(int port)
{
#ifdef modbus_print_msg_info
    printf("initial the modbus server...\n");
#endif
    int s = -1;
    int modbus_times=0;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    ctx = modbus_new_tcp(NULL, port);
    mb_mapping = modbus_mapping_new(modbus_mapping_1,modbus_mapping_2,modbus_mapping_3,modbus_mapping_4);
    if (mb_mapping == NULL)
    {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",modbus_strerror(errno));
        modbus_free(ctx);
    }

    s = modbus_tcp_listen(ctx,10);
    if(s == -1)
    {
        modbus_mapping_free(mb_mapping);
        modbus_free(ctx);
        //return;
    }

    int rc=0;
    while(1)
    {
        rc=modbus_tcp_accept(ctx,&s);

        while(rc!=-1)
        {
            modbus_times++;
          //receive the modbus data
            uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
            if(rc==-1)
            {
#ifdef modbus_print_error_msg_info
            printf("modbus accept fail!!!\n");
#endif
            }
            else
            {
#ifdef modbus_print_msg_info
                printf("modbus accept success %d times...\n",modbus_times);
#endif
            }
            std::unique_lock<std::mutex> lock(modbus_mtx);
            rc=modbus_receive(ctx,query);
            if(rc>0)
            {
#ifdef modbus_print_data_info
                //int dianhan=mb_mapping->tab_registers[0x103];
                //printf("dianhan:%d\n",dianhan);
#endif
                set_position_x=data_splicing(mb_mapping->tab_registers[modbus_robot_set_x_l],mb_mapping->tab_registers[modbus_robot_set_x_h]);//l&h
                set_position_y=data_splicing(mb_mapping->tab_registers[modbus_robot_set_y_l],mb_mapping->tab_registers[modbus_robot_set_y_h]);//y
                set_position_z=data_splicing(mb_mapping->tab_registers[modbus_robot_set_z_l],mb_mapping->tab_registers[modbus_robot_set_z_h]);//z
                set_posture_rx=data_splicing(mb_mapping->tab_registers[modbus_robot_set_rx_l],mb_mapping->tab_registers[modbus_robot_set_rx_h]);//rx
                set_posture_ry=data_splicing(mb_mapping->tab_registers[modbus_robot_set_ry_l],mb_mapping->tab_registers[modbus_robot_set_ry_h]);//ry
                set_posture_rz=data_splicing(mb_mapping->tab_registers[modbus_robot_set_rz_l],mb_mapping->tab_registers[modbus_robot_set_rz_h]);//rz
                movel_speed=data_splicing(mb_mapping->tab_registers[modbus_robot_move_speed_l],mb_mapping->tab_registers[modbus_robot_move_speed_h]);
                //robot_choose=mb_mapping->tab_registers[modbus_robot_choose];
                robot_tcp_choose=mb_mapping->tab_registers[modbus_robot_tcp];
                move_function=mb_mapping->tab_registers[modbus_robot_move_func];
                ip_1=mb_mapping->tab_registers[modbus_robot_ip_1];
                ip_2=mb_mapping->tab_registers[modbus_robot_ip_2];
                ip_3=mb_mapping->tab_registers[modbus_robot_ip_3];
                ip_4=mb_mapping->tab_registers[modbus_robot_ip_4];
                robot_software_stop=mb_mapping->tab_registers[modbus_robot_stop_model];
                robot_hour=mb_mapping->tab_registers[modbus_robot_hour];
                robot_minute=mb_mapping->tab_registers[modbus_robot_minute];
                robot_second=mb_mapping->tab_registers[modbus_robot_second];
                robot_msecond=mb_mapping->tab_registers[modbus_robot_msecond];

                modbus_reply(ctx, query, rc, mb_mapping);
                read_upmachine_flag=1;
#ifdef modbus_print_data_info
                printf("robot_choose:=%d\n",robot_choose);
                printf("set_position_x:=%f\n",set_position_x);
                printf("set_position_y:=%f\n",set_position_y);
                printf("set_position_z:=%f\n",set_position_z);
                printf("set_posture_rx:=%f\n",set_posture_rx);
                printf("set_posture_ry:=%f\n",set_posture_ry);
                printf("set_posture_rz:=%f\n",set_posture_rz);
#endif
#ifdef modbus_print_msg_info
                printf("read the data from upmachine done...\n");
#endif
            }
            else if(rc==-1)
            {
                //close(s);
                continue;
            }
            // update the modbus data
            if(update_flag)
            {
                mb_mapping->tab_registers[modbus_robot_read_x_h]=data_divide_h(&read_position_x);// x
                mb_mapping->tab_registers[modbus_robot_read_x_l]=data_divide_l(&read_position_x);
                mb_mapping->tab_registers[modbus_robot_read_y_h]=data_divide_h(&read_position_y);// y
                mb_mapping->tab_registers[modbus_robot_read_y_l]=data_divide_l(&read_position_y);
                mb_mapping->tab_registers[modbus_robot_read_z_h]=data_divide_h(&read_position_z);// z
                mb_mapping->tab_registers[modbus_robot_read_z_l]=data_divide_l(&read_position_z);
                mb_mapping->tab_registers[modbus_robot_read_rx_h]=data_divide_h(&read_posture_rx);// rx
                mb_mapping->tab_registers[modbus_robot_read_rx_l]=data_divide_l(&read_posture_rx);
                mb_mapping->tab_registers[modbus_robot_read_ry_h]=data_divide_h(&read_posture_ry);// ry
                mb_mapping->tab_registers[modbus_robot_read_ry_l]=data_divide_l(&read_posture_ry);
                mb_mapping->tab_registers[modbus_robot_read_rz_h]=data_divide_h(&read_posture_rz);// rz
                mb_mapping->tab_registers[modbus_robot_read_rz_l]=data_divide_l(&read_posture_rz);
                mb_mapping->tab_registers[modbus_robot_state]=robot_state; // robot_state wheather fast_stop
                mb_mapping->tab_registers[modbus_fifo_array]=fifo_position.size();//back the point in fifo to upmachine
#ifdef modbus_print_msg_info
                printf("update the register...\n");
#endif
            //update_flag=0;
            }
            lock.unlock();
            usleep(5000);
        }
    }
    printf("Quit the loop: %s\n", modbus_strerror(errno));
    if (s != -1)
    {
        close(s);
    }
    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);
}

void modbus_zenturm::modbus_data_communication()
{
    while(1)
    {
        // set the data read from robot through the read_position_x to the modbus
        read_position_x=robot_position_x;
        read_position_y=robot_position_y;
        read_position_z=robot_position_z;
        read_posture_rx=robot_posture_rx;
        read_posture_ry=robot_posture_ry;
        read_posture_rz=robot_posture_rz;
#ifdef modbus_print_data_info
        printf("modbus_read_position_x:=%f\n",read_position_x);
        printf("modbus_read_position_y:=%f\n",read_position_y);
        printf("modbus_read_position_z:=%f\n",read_position_z);
        printf("modbus_read_posture_rx:=%f\n",read_posture_rx);
        printf("modbus_read_posture_ry:=%f\n",read_posture_ry);
        printf("modbus_read_posture_rz:=%f\n",read_posture_rz);
#endif

        // set the data read from up machine to the fifo
        std::unique_lock<std::mutex> lock(fifo_lock);
        if(read_upmachine_flag)
        {
            fifo_write();
            read_upmachine_flag=0;
        }
        lock.unlock();
        usleep(50000);
        robot_ip=ip_1;
        robot_ip+=".";
        robot_ip=ip_2;
        robot_ip+=".";
        robot_ip=ip_3;
        robot_ip+=".";
        robot_ip=ip_4;
    }
}

void modbus_zenturm::modbus_client(std::string server_ip,int server_port)
{
#ifdef modbus_print_msg_info
    printf("initial the modbus client...\n");
#endif
    modbus_t* ctx;
    //modbus_mapping_t* mb_mapping;
    ctx=modbus_new_tcp(server_ip.c_str(),server_port);

    int ret = -1;
    while(1)
    {
        if (modbus_connect(ctx) == -1)
        {
            fprintf(stderr,"Connection failed: %s\n",modbus_strerror(errno));
            modbus_free(ctx);
            continue;
        }
        while(1)
        {
            uint16_t array[3];
            ret = modbus_read_registers(ctx,modbus_robot_read_x_h,2,array);
            std::cout<<"array:=("<<array[0]<<","<<array[1]<<")"<<ret<<std::endl;
        }
    }
}

void modbus_zenturm::fifo_write()
{
    cv::Point3f temp_position(set_position_x,set_position_y,set_position_z);
    cv::Point3f temp_posture(set_posture_rx,set_posture_ry,set_posture_rz);
    int fifo_size=fifo_position.size();
    if(fifo_size>1)
    {
        float _x=fifo_position[fifo_size-1].x-set_position_x;
        float _y=fifo_position[fifo_size-1].y-set_position_y;
        float _z=fifo_position[fifo_size-1].z-set_position_z;
        float _rx=fifo_posture[fifo_size-1].x-set_posture_rx;
        float _ry=fifo_posture[fifo_size-1].y-set_posture_ry;
        float _rz=fifo_posture[fifo_size-1].z-set_posture_rz;

        float position_offset=sqrt(_x*_x+_y*_y+_z*_z);
        float posture_offset=sqrt(_rx*_rx+_ry*_ry+_rz*_rz);

        bool empty_flag=0;
        if(fifo_size<5)
        {
            empty_flag=1;
        }
        else
        {
            empty_flag=0;
        }

        if(position_offset>fifo_position_threshold||posture_offset>fifo_posture_threshold||empty_flag)
        {
            if(fifo_size<fifo_max)
            {
                fifo_position.push_back(temp_position);
                fifo_posture.push_back(temp_posture);
            }
            else
            {
#ifdef modbus_print_error_msg_info
                printf("das fifo ist full...\n");
#endif
            }
        }
    }
    else
    {
        fifo_position.push_back(temp_position);
        fifo_posture.push_back(temp_posture);
    }

#ifdef modbus_print_data_info
    printf("fifo size ist %d\n",fifo_size);
#endif
}
