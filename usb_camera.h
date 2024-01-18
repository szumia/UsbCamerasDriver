//
// Created by lzt on 2024-1-13.
// usb相机驱动
//

#ifndef OMNI_USB_CAMERA_H
#define OMNI_USB_CAMERA_H

#include <iostream>
#include <csignal>
#include <chrono>
#include <string>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>
#include "global_space.hpp"
#include "third_party/tinyxml2.h"
#include "process.h"
#include "usb_camera_defines.h"


enum UsbCamerastate{
    IDLE,           //闲置
    RUNNING,
    ERROR
};

class UsbCamera
{
private:


    void set_camera_state(int camera_state)
    {
        this->camera_state_ = camera_state;
    }
    void set_exposure(int exposure)
    {
        this->exposure_ = exposure;
    }
    void set_fps(int fps)
    {
        this->fps_ = fps;
    }
    void set_resolution_width(int resolution_width)
    {
        this->resolution_width_ = resolution_width;
    }
    void set_resolution_height(int resolution_height)
    {
        this->resolution_height_ = resolution_height;
    }
    void set_brightness(int brightness)
    {
        this->brightness_ = brightness;
    }


public:
    UsbCamera(){};
    UsbCamera(int id)
    {
        this->id_ = id;
    };
    ~UsbCamera(){
        std::cout<<"usb camera "<< this->get_id()<<" release."<<std::endl;
        this->close_camera();
    };

    bool set_dev_path(const std::string &path);
    bool set_cameraMartix(const std::string & camera_martix_str);
    bool set_distCoeffs(const std::string  & distcoeffs_str);

    bool close_camera();
    bool open_camera();
    bool reopen_camera();
    bool init_camera_params(int resolution_width,int resolution_height,int fps,int exposure,int brightness);

    void show_camera_info();
    bool change_camera_params(int resolution_width,int resolution_height,int fps,int exposure,int brightness);
    bool get_img(cv::Mat & dst);
    bool update_img();

    //自动取图，录视频等
    void auto_recordImgs();
    void manual_recordImgs();
    void recordVideo();

    void set_id(int id)
    {
        this->id_ = id;
    }
    int get_id()
    {
        return this->id_;
    }
    int get_fps()
    {
        return this->fps_;
    }
    int get_resolution_width()
    {
        return this->resolution_width_;
    }
    int get_resolution_height()
    {
        return this->resolution_height_;
    }
    int get_camera_state()
    {
        return this->camera_state_;
    }
    int get_exposure()
    {
        return this->exposure_;
    }

    int get_brightness()
    {
        return this->brightness_;
    }
private:

    int camera_state_;
    int exposure_;
    int fps_;
    int resolution_width_,resolution_height_;
    int brightness_;

public:
    cv::Mat src_;
    cv::VideoCapture cap_;
    std::string dev_path;
    cv::Mat cameraMartix, distCoeffs;       //相机内参

    int id_;
    int thread_id = -1;         //线程号
    long long frame_id = 0;     //自己取图帧戳
    long long global_frame_id;  //全向感知帧戳
    int reopen_times = 0;       //重新打开次数
    int error_times = 0;        //打开失败的次数
    int grab_error_times = 0;       //抓取失败帧数
    int regrab_times = 0;        //启动次数
    int reconnect_times = 0;     //重连次数
    bool xml_init = false;                                                //是否由xml文件初始化了

    //进程控制
    volatile bool is_exec = true;
    volatile long long process_index = 0;
    volatile long long receive_index = 0;
    volatile long long record_index = 0;
    volatile long long photo_index = 0;
};



class UsbCamerasManager
{
private:


public:
    UsbCamerasManager();
    ~UsbCamerasManager(){
        delete xmldoc;
        delete [] usb_cameras;
        delete [] processers;
        delete [] imgs;
    };

    int load_cfg_xmlfile();
    tinyxml2::XMLElement* get_UsbCameraNode(tinyxml2::XMLElement*root,const std::string & id);
    bool exec_usb_cameras_threads();
    void receive_thread(const int& id);
    void receive2_thread(const int& id);
    void process_thread(const int& id);
    void process2_thread();
    void record_thread(const int &id);

    void exec_auto_photos();            //手动拍照
    void exec_manual_photos();          //自动拍照

   static  void signal_handler(int sign)
   {
       isrun = false;
   }

private:


public:
    int enable_cameras_id[USB_CAMERA_MAX_SIZE];                         //可用的USB相机索引，-1表示不可用
    int threads_count = 0;                                              //根据启动成功的相机制定线程数量
    tinyxml2::XMLDocument *xmldoc;
    UsbCamera *usb_cameras;
    std::vector<cv::Mat> img_vec;
    static bool isrun;                                         //Linux对全部进程的控制信号

    PROCESS::Processer *processers;                    //一个线程一个ad
    std::mutex threads_mutexs[USB_CAMERA_MAX_SIZE];                     //一个线程一个锁
    cv::Mat *imgs;                                                      //一个线程一个img（多取图多处理） or 一个Usb一个img（多取图单处理）

};

//保存视频类
class Omni_SaveVideo {
public:
    cv::Size size;
    Omni_SaveVideo(){;}
    Omni_SaveVideo(const UsbCamera &usb_camera);

    ~Omni_SaveVideo();

    void updateImage(cv::Mat &img);

    bool getState() {
        return state_;
    }

private:
    cv::VideoWriter video_writer_;
    bool state_ = false;
    int frame_count = 0;
    void open_video_writer();
    UsbCamera record_usb_camera;

};
#endif //OMNI_USB_CAMERA_H
