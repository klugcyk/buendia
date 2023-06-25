/*
    文件等级：密一
    author:klug
    献给美人儿蕾梅黛丝
    start:230524
    last:230529
*/

#include "gene_operate/json_operate.hpp"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QByteArray>

namespace mein_json
{

/*
    json文件读取
*/
void json_read(std::string filename)
{
    QFile file(QString::fromStdString(filename));
    if(!file.open(QIODevice::ReadOnly| QIODevice::Text))
    {
        return;
    }

    QByteArray array = file.readAll(); //将数据读到array中

    QJsonParseError json_error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(array,&json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
        QJsonObject rootObj = jsonDoc.object();
        QJsonArray  JsonArray;
        QString str;

        if(rootObj.contains("camera_param") && rootObj.value("camera_param").isObject())
        {
            QJsonObject childJson=rootObj.value("camera_param").toObject();

            for (int i = 0; i<childJson.size();i++)
            {
                double A=childJson.value("A").toDouble();
            }
            /*JsonArray =  childJson.value("A").toArray();

            for (int i = 0; i<JsonArray.size();i++)
            {
                QJsonObject childObj = JsonArray[i].toObject();

                if(childObj.contains("first") && childObj.value("first").isString())
                {
                    str =  childObj.value("first").toString();
                     qDebug() <<  str ;
                }

                if(childObj.contains("second") && childObj.value("second").isString())
                {
                    str =  childObj.value("second").toString();
                    qDebug() <<  str ;
                }
            }*/
        }
    }
}

/*
    json文件保存
*/
void json_write(std::string filename)
{
    QVariantHash data;

    QVariantHash subData1;
    subData1.insert("name", "apple");
    subData1.insert("icon", "appleIcon");
    subData1.insert("describe", "an apple");

    data.insert("first fruit", subData1);

    QVariantHash subData2;
    subData2.insert("name", "orange");
    subData2.insert("icon", "orangeIcon");
    subData2.insert("describe", "an orange");
    data.insert("second fruit", subData2);

    QJsonArray array1;
    for (int i = 0; i < 5; i++)
    {
        array1.insert(i, QString::fromLocal8Bit("eat %1").arg(i));
    }

    data.insert("three fruit array", array1);

    QJsonObject rootObj = QJsonObject::fromVariantHash(data);
    QJsonDocument document;
    document.setObject(rootObj);

    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);
    //根据实际填写路径
    QString name=QString::fromStdString(filename);
    QFile file(name);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "file error!";
    }
    QTextStream in(&file);
    in << json_str;

    file.close();   // 关闭file
}

/*
    json文件写入，三维重建中使用
    @filename:写入文件名
    @name:名称首地址
    @data:数据首地址
    @length1:单个类型长度长度
    @length2:写入个数，长度
*/
void json_write_construct(std::string filename,char *name,double *data,int length1,int length2)
{
    char get_name[length1];
    double get_data;
    QString save_tpye="type";
    QVariantHash jsondata;
    QVariantHash subData1;

    for(int j=0;j<length2;j++)
    {
        for(int i=0;i<length1;i++)
        {
            get_name[i]=*name;
            name++;
        }
        get_data=*data;

        if(get_name[0]=='t'&&get_name[1]=='y'&&get_name[2]=='p'&&get_name[3]=='e')
        {
            if(get_data==1)//单目结构光
            {
                save_tpye="ein_camera";
            }
            else if(get_data==2)//双目结构光
            {
                save_tpye="zwei_camera";
            }
        }

        data++;

        subData1.insert(get_name,get_data);
        jsondata.insert(save_tpye,subData1);
/*
        QJsonArray array1;
        for (int i = 0; i < 5; i++)
        {
            array1.insert(i, QString::fromLocal8Bit("eat %1").arg(i));
        }

        data.insert("three fruit array", array1);
*/
        QJsonObject rootObj = QJsonObject::fromVariantHash(jsondata);
        QJsonDocument document;
        document.setObject(rootObj);

        QByteArray byte_array=document.toJson(QJsonDocument::Compact);
        QString json_str(byte_array);
        //根据实际填写路径
        QString name=QString::fromStdString(filename);
        QFile file(name);

        if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            qDebug() << "file error!";
        }
        QTextStream in(&file);
        in << json_str;

        file.close();   // 关闭file
    }
}

/*
    json文件读取
    @filename:读取文件名
    @name:名称首地址
    @data:数据首地址
    @length:读取个数，长度
*/
void json_read_construct(std::string filename,char *name,char *data,int length)
{

}

/*
    按需求读取json文件中的数据
    @filename:读取的json文件
    @json_get_name:需要获取json中包含的数据名称
    @json_get_value:获取json数据内容
*/
void json_read(std::string filename,std::string *json_name,double *json_value)
{
    QFile file(QString::fromStdString(filename));
    if(!file.open(QIODevice::ReadOnly| QIODevice::Text))
    {
        return;
    }

    QByteArray array = file.readAll(); //将数据读到array中

    QJsonParseError json_error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(array,&json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
        QJsonObject rootObj = jsonDoc.object();
        QJsonArray  JsonArray;
        QString str;

        if(rootObj.contains(QString::fromStdString(*json_name)) && rootObj.value(QString::fromStdString(*json_name)).isObject())
        {
            QJsonObject childJson=rootObj.value(QString::fromStdString(*json_name)).toObject();

            for (int i=0;i<childJson.size()-1;i++)
            {
                json_name++;
                json_value++;
                *json_value=childJson.value(QString::fromStdString(*json_name)).toDouble();
            }
        }
    }
}

/*
    写入json文件
    @filename:写入的文件名称
    @json_name:写入的变量名称
    @json_value:写入的变量数值
    @save_num:需要保存数据的个数
*/
void json_write(std::string filename,std::string *json_name,double *json_value,int save_num)
{
    QVariantHash jsondata;
    QVariantHash subData1;
    QString save_tpye=QString::fromStdString(*json_name);

    for(int i=0;i<save_num-1;i++)
    {
        json_name++;
        json_value++;
        subData1.insert(QString::fromStdString(*json_name),*json_value);
        jsondata.insert(save_tpye,subData1);

        QJsonObject rootObj = QJsonObject::fromVariantHash(jsondata);
        QJsonDocument document;
        document.setObject(rootObj);

        QByteArray byte_array=document.toJson(QJsonDocument::Compact);
        QString json_str(byte_array);
        //根据实际填写路径
        QString name=QString::fromStdString(filename);
        QFile file(name);

        if (!file.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            qDebug() << "file error!";
        }
        QTextStream in(&file);
        in << json_str;

        file.close();   // 关闭file
    }
}

};
