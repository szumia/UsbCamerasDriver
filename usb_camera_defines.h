//
// Created by lzt on 2024-1-13.
// 定义了usb相机的宏定义
//

#ifndef OMNI_USB_CAMERA_DEFINES_H
#define OMNI_USB_CAMERA_DEFINES_H


#define USB_CAMERA_CFG_PATH "../config/usb_camera_cfg.xml"
#define USB_CAMERA_VIDEOS_PATH "videos"            //存放录制的视频的文件路径
#define USB_CAMERA_MAX_REOPEN_TIMES 10                  //最大重启次数
#define USB_CAMERA_MAX_GRAB_TIMES 50                 //最大连续取图失败次数
#define USB_CAMERA_MAX_SIZE 20
#define USB_CAMERA_RECORD_FPS 30                            //录制FPS
#define USB_CAMERA_RECORD_MAX_FRAMES  10000                 //最大录制帧数
#define USB_CAMERA_MAX_PHOTOS  25                      //最大拍照张数

#define USB_CAMERAS                                         //开启相机
//#define USB_CAMERA_RECORD_VIDEOS                            //开启多线程录制视频
#define USB_CAMERA_MUTIPLE_THREADS                          //开启多处理线程
//#define USB_CAMERA_AUTO_PHOTOS                              //开启多线程自动拍照
//#define USB_CAMERA_MANUAL_PHOTOS                            //开启多线程手动拍照












#endif //OMNI_USB_CAMERA_DEFINES_H
