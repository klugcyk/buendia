/*
    文件等级：密一
    author:klug
    献给美人儿蕾梅黛丝
    start:230524
    last:230626
*/

#ifndef json_operate_hpp
#define json_operate_hpp

#include <string>
#include <iostream>

namespace mein_json
{

void json_write(std::string filename);
void json_read(std::string filename);
void json_read(std::string filename,std::string *json_name,double *json_value);
void json_write(std::string filename,std::string *json_name,double *json_value,int save_num);
void json_write(std::string filename,char *json_name,double *json_value,int save_num,int length);
void json_write_construct(std::string filename,char *name,double *data,int length1,int length2);
void json_read_construct(std::string filename,char *name,char *data);

};

#endif
