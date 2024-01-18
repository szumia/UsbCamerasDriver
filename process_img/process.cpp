#include "process.h"

namespace PROCESS
{

void Processer::processer_ctrl(cv::Mat& img,const int & id)
{
    this->img = img;
    this->usb_camera_id = id;

    if(!img.empty())
    {

#ifdef Show_img
      std::string str ="usb "+std::to_string(id);
      cv::namedWindow(str,cv::WINDOW_AUTOSIZE);
      cv::imshow(str, this->img);
      cv::waitKey(1);
#endif


    } else{;}
}




};
