/*
    文件等级：密一
    author:klug
    献给恩师阿尔瓦罗赛佩达萨幕迪奥
    start:230323
    last:230325
*/

#include "galvo_control/galvo_control.hpp"

galvo_control::galvo_control()
{
#ifdef galvo_control_msg_print_info
    printf("start the galvo control...\n");
#endif
    //socket_accept=socket_server_initial(galvo_port,galvo_ip);

}

galvo_control::~galvo_control()
{
#ifdef galvo_control_msg_print_info
    printf("close the galvo control...\n");
#endif

}

/*
    控制两个振镜运动
    @angle_ein:第一个振镜旋转的角度
    @angle_zwei:第二个振镜旋转的角度
    @galvo_state:振镜的状态
*/
int galvo_control::galvo_rotate(float angle_ein,float angle_zwei)
{
    static int galvo_state=0;
    short int _ein,_zwei;

    ein_line_b=(ein_galvo_max+ein_galvo_min)/2;
    ein_line_k=(digital_galvo_max-ein_line_b)/ein_galvo_max;
    _ein=(int)(ein_line_k*angle_ein+ein_line_b);

    zwei_line_b=(zwei_galvo_max+zwei_galvo_min)/2;
    zwei_line_k=(digital_galvo_max-zwei_line_b)/zwei_galvo_max;
    _zwei=(int)(zwei_line_k*angle_zwei+zwei_line_b);

#ifdef galvo_control_data_print_info
    printf("ein_galvo_s:=%d\n",_ein);
    printf("zwei_galvo_s:=%d\n",_zwei);
#endif

    //数据头
    sendBuf[0]=0xa2;
    sendBuf[1]=0x00;
    sendBuf[2]=0x09;

    //数据
    unsigned char *pc;

    if(_ein>=0)
    {
        pc=(unsigned char *)(&_ein);
        sendBuf[3]=0x00; //galvo ein
        sendBuf[4]=0x00;
        sendBuf[6]=*pc;
        pc++;
        sendBuf[5]=*pc;
    }
    else
    {
        pc=(unsigned char *)(&_ein);
        sendBuf[3]=0xff; //galvo ein
        sendBuf[4]=0xff;
        sendBuf[6]=*pc;
        pc++;
        sendBuf[5]=*pc;
    }

    if(_zwei>=0)
    {
        pc=(unsigned char *)(&_zwei);
        sendBuf[7]=0x00; //galvo zwei
        sendBuf[8]=0x00;
        sendBuf[10]=*pc;
        pc++;
        sendBuf[9]=*pc;
    }
    else
    {
        pc=(unsigned char *)(&_zwei);
        sendBuf[7]=0xff; //galvo zwei
        sendBuf[8]=0xff;
        sendBuf[10]=*pc;
        pc++;
        sendBuf[9]=*pc;
    }

    //响应位
    sendBuf[11]=0x01; //galvo response

    //校验
    unsigned short crc;
    Platform::Communication::Crc16Class c;
    crc=c.crc16(sendBuf,12);
    unsigned char *p;
    p=(unsigned char*)&crc;
    sendBuf[12]=*p;
    p++;
    sendBuf[13]=*p;

    //发送
    galvo_state=send(socket_accept,sendBuf,14,0);

    return galvo_state;
}

/*
    socket读取振镜角度
*/
void galvo_control::galvo_read()
{
    sendBuf[0]=0xa2;
    sendBuf[1]=0x03;
    sendBuf[2]=0x00;
    sendBuf[3]=0xd0;
    sendBuf[4]=0xd2;

    send(socket_accept,sendBuf,5,0);
    //usleep(1000);
    if(recv(socket_accept,recvBuf,13,0)>0)
    {
        if(recvBuf[1]==0x03&&recvBuf[2]==0x08)
        {
            char temp[2];
            temp[0]=recvBuf[6];
            temp[1]=recvBuf[5];
            short int *ein_=(short int*)temp;
#ifdef galvo_control_data_print_info
            printf("ein_galvo_r:=%d\n",*ein_);
#endif
            ein_galvo_angle=((*ein_)-ein_line_b)/ein_line_k;

            temp[0]=recvBuf[10];
            temp[1]=recvBuf[9];
            short int *zwei_=(short int*)temp;
#ifdef galvo_control_data_print_info
            printf("zwei_galvo_r:=%d\n",*zwei_);
#endif
        zwei_galvo_angle=((*zwei_)-zwei_line_b)/zwei_line_k;
        }
        else
        {
#ifdef galvo_control_error_print_info
            printf("recv data not correct!!!\n");
#endif
        }
    }
}
