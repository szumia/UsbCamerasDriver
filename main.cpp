#include <iostream>
#include "usb_camera.h"


bool UsbCamerasManager::isrun = true;          //初始化全局进程中断控制信号


int main(int argc, char *argv[]) {


//    //单独启用相机
//    UsbCamera c1(1);
//    c1.set_dev_path("/dev/video_cam_01");
//    c1.init_camera_params(640, 480, 30, 15, -10);
//    c1.open_camera();
//    cv::Mat img;
////    bool grab_success1;
////    while (1)
////    {
////        c1.update_img();
////        grab_success1=c1.get_img(img);
////        if(grab_success1)
////        {
////            cv::namedWindow("cap1",cv::WINDOW_AUTOSIZE);
////            cv::imshow("cap1",img);
////            cv::waitKey(1);
////        }
////
////    }
////    ////录制视频
////    c1.recordVideo();
////    //自动拍照
////    c1.auto_recordImgs();
////    //手动拍照
////    c1.manual_recordImgs();


   //推荐：启用相机管家
    UsbCamerasManager usb_cameras_manager_;

#ifdef USB_CAMERA_AUTO_PHOTOS
    usb_cameras_manager_.exec_auto_photos();
#elif defined(USB_CAMERA_MANUAL_PHOTOS)
    usb_cameras_manager_.exec_manual_photos();
#else
    usb_cameras_manager_.exec_usb_cameras_threads();
#endif

    return 0;
}
