#ifndef OMNI_PROCESS_H
#define OMNI_PROCESS_H

#include <iostream>
#include <opencv4/opencv2/opencv.hpp>

#include "process_defines.h"

namespace PROCESS
{

    //TODO:: write your image process class here
    class Processer{
    public:
        Processer(){};
        ~Processer(){};
        void processer_ctrl(cv::Mat& img,const int & id);
    private:



    public:
        cv::Mat img;
        int usb_camera_id;
    private:
    };





}


#endif //OMNI_PROCESS_H
