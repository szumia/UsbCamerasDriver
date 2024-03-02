#include "usb_camera.h"


bool UsbCamera::set_dev_path(const std::string &path)
{
    this->dev_path = path;
    return true;
}

bool UsbCamera::set_cameraMartix(const std::string & camera_martix_str)
{
    std::vector<double> double_nums;
    double num;
    std::istringstream ss(camera_martix_str);
    std::string token;
    while (std::getline(ss,token,','))
    {
        try {
            num =std::stod(token);
            double_nums.push_back(num);
        }catch (std::invalid_argument const& e)
        {
            continue;
        }catch(std::out_of_range const& e){
            continue;
        }
    }
//        std::cout<<"size:"<<double_nums.size()<<std::endl;

    if(double_nums.size() != 9)
    {
        std::cout<<"Error:usb camera cameraMartix has params 'size uneauql to 9. "<<std::endl;
        return false;
    } else
    {
        this->cameraMartix =(cv::Mat_<double>(3,3)<<double_nums[0],double_nums[1],double_nums[2],
                double_nums[3],double_nums[4],double_nums[5],
                double_nums[6],double_nums[7],double_nums[8]);
    }

    return true;
};


bool UsbCamera::set_distCoeffs(const std::string  & distcoeffs_str)
{
    std::vector<double> double_nums;
    double num;
    std::istringstream ss(distcoeffs_str);
    std::string token;
    while (std::getline(ss,token,','))
    {
        try {
            num =std::stod(token);
            double_nums.push_back(num);
        }catch (std::invalid_argument const& e)
        {
            continue;
        }catch(std::out_of_range const& e){
            continue;
        }
    }
//        std::cout<<"size:"<<double_nums.size()<<std::endl;

    if(double_nums.size() != 5)
    {
        std::cout<<"Error:usb camera distcoeffs has params'size uneauql to 5. "<<std::endl;
        return false;
    } else
    {
        this->distCoeffs = (cv::Mat_<double>(5,1)<<double_nums[0],double_nums[1],double_nums[2],double_nums[3],double_nums[4]);
    }

    return true;
};


bool UsbCamera::close_camera()
{
    if(this->cap_.isOpened())
    {
        this->cap_.release();
        this->camera_state_ = UsbCamerastate::IDLE;
        std::cout<<"close usb camera "<< this->get_id()<<" ."<<std::endl;
    } else{;}
}


bool UsbCamera::open_camera()
{
    if(this->cap_.isOpened())
    {
        std::cout<<"Usb camera device "<< this->dev_path<<" has already opened."<<std::endl;
    } else
    {
        this->cap_.open(this->dev_path);

        if(this->cap_.isOpened())
        {
            std::cout<<"Open usb camera device "<< this->dev_path<<" successd."<<std::endl;
            this->cap_.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
            this->cap_.set(cv::CAP_PROP_FRAME_WIDTH, this->get_resolution_width());
            this->cap_.set(cv::CAP_PROP_FRAME_HEIGHT, this->get_resolution_height());
            this->cap_.set(cv::CAP_PROP_FPS, this->get_fps());//帧数30
            this->cap_.set(cv::CAP_PROP_BRIGHTNESS, this->get_brightness());
            this->cap_.set(cv::CAP_PROP_EXPOSURE, this->get_exposure());
            this->camera_state_ = UsbCamerastate::RUNNING;
            this->show_camera_info();
            return true;
        } else{
            std::cout<<"Open usb camera device "<< this->dev_path<<" failed."<<std::endl;
            this->error_times++;
        }
    }
    return false;
}

bool UsbCamera::reopen_camera()
{
    if(this->cap_.isOpened())
    {
        this->cap_.release();
    } else{;}

    return this->open_camera();
}

bool UsbCamera::init_camera_params(int resolution_width,int resolution_height,int fps,int exposure,int brightness)
{
    this->set_exposure(exposure);
    this->set_fps(fps);
    this->set_resolution_width(resolution_width);
    this->set_resolution_height(resolution_height);
    this->set_brightness(brightness);
}

//-1则表示不改变
bool UsbCamera::change_camera_params(int resolution_width,int resolution_height,int fps,int exposure,int brightness)
{
    if(exposure != -1)
    {
        this->set_exposure(exposure);
    }

    if(fps!=-1)
    {
        this->set_fps(fps);
    }

    if(resolution_width != -1)
    {
        this->set_resolution_width(resolution_width);
    }

    if(resolution_height != -1)
    {
        this->set_resolution_height(resolution_height);
    }

    if(brightness != -1)
    {
        this->set_brightness(brightness);
    }

    return true;
}



bool UsbCamera::get_img(cv::Mat & dst)
{
    if(this->src_.empty())
    {
        return false;
    } else
    {
        dst = this->src_.clone();
//        dst = this->src_;
    }
    return true;
}

bool UsbCamera::update_img()
{
    bool flag = true;
    this->frame_id ++;

    //Method1:抓取一帧解码一帧
    this->cap_>>this->src_;
    if(this->src_.empty() && this->grab_error_times <USB_CAMERA_MAX_GRAB_TIMES)
    {
        this->grab_error_times++;
        std::cout<<"FAILED:usb camera "<< this->get_id()<<" grab img "<< this->frame_id<<" failed."<<std::endl;
        flag = false;
    }
    else if(!this->src_.empty())
    {
        this->grab_error_times = 0;
    }

//    //相机中途可能发生断连，只能重新拔插，所以干脆关闭该相机（无效）
//    if(this->grab_error_times > USB_CAMERA_MAX_GRAB_TIMES)
//    {
//        std::cout<<999<<std::endl;
//        while(!(this->reopen_camera()) && this->reopen_times < USB_CAMERA_MAX_REOPEN_TIMES)
//        {
//            this->reopen_times++;
//            std::cout<<"Reopen usb camera device"<< this->dev_path<<" "<<this->reopen_times<<" times..."<<std::endl;
//        }
//        if(this->reopen_times == USB_CAMERA_MAX_REOPEN_TIMES)
//        {
//            std::cout<<std::endl;
//            this->reopen_times = 0;
//            this->reconnect_times ++;
//        }
//
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));        //延迟500ms
//    }
//    //如果重启次数过多，关闭相机,实际上无效
//    if(this->reconnect_times >= USB_CAMERA_MAX_REOPEN_TIMES) {
//        std::cout<<1111<<std::endl;
//        this->close_camera();
//    }


    /*
    //Method2:连续抓取3帧，只解码返回最后一帧
    this->cap_.grab();
    this->cap_.grab();
    bool grab_success = this->cap_.grab();
    if(!grab_success)
    {
        this->grab_error_times++;
        std::cout<<"FAILED:usb camera "<< this->get_id()<<" grab img "<< this->frame_id<<" failed."<<std::endl;
        return false;
    } else
    {
        this->cap_.retrieve(this->src_);
    }
    */

    return flag;
}

//自动拍照
void UsbCamera::auto_recordImgs()
{
    std::string str = "autoSavedPictures_"+std::to_string(this->get_id())+"_";
    std::string savedFolderName  = Global_Space::setFolderPath(str, 0);
    //TODO::set auto photo params
    int click_down          = 5;                    //开始取图倒计时  (s)
    //保存一张耗时 = 取图间隔 * 取图速率
    int interval            = 2;                    //取图间隔
    int getImgRate          = 300;                  //取图速率
    int imgs_sum_now        = 0;
    int img_order           = 0;
    std::string  img_name;

    std::string window_name = "AutoRecordPictures "+std::to_string(this->get_id());
    //cv::namedWindow("AutoRecordPictures",cv::WINDOW_NORMAL);
    cv::namedWindow(window_name,0);
    cv::Mat src;

    //准备时间
    while (click_down > 0)
    {
        double  time = 0;
        while (1)
        {
            auto  start = std::chrono::high_resolution_clock ::now();
            while (!this->update_img()){;}      //一直取图直到成功
            src = this->src_;

            //下面可以加上图像处理
            //this->dealImg();

            cv::putText(src, std::to_string(click_down), cv::Point(src.cols/2,src.rows/2),
                        cv::FONT_HERSHEY_SIMPLEX,5,cv::Scalar(0,0,255),4);
            cv::imshow(window_name,src);
            cv::waitKey(1);

            auto  end = std::chrono::high_resolution_clock ::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
            time += duration.count();
            if(time>=1000)
            {
                break;
            }
        }
        click_down --;
    }

    int tmp_interval = interval;
    while (imgs_sum_now < USB_CAMERA_MAX_PHOTOS)
    {
        while (interval > 0)
        {
            double  time = 0;
            while (1)
            {
                auto  start = std::chrono::high_resolution_clock ::now();
                while (!this->update_img()){;}      //一直取图直到成功
                src = this->src_;
                //可以加上图像处理
                //this->dealImg();

//                cv::putText(src, to_string(interval), cv::Point(50,50),
//                            FONT_HERSHEY_SIMPLEX,2,cv::Scalar(0,255,0),3);
                cv::imshow(window_name,src);
                cv::waitKey(1);

                auto  end = std::chrono::high_resolution_clock ::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
                time += duration.count();
                if(time >= getImgRate)
                {
                    break;
                }
            }
            interval --;
        }

        cv::putText(src, std::to_string(img_order) + ".jpg", cv::Point(30,30),
                    cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,255,0),2);
        cv::imshow(window_name,src);
        cv::waitKey(1);

        //save pictures
        img_name ="./" + savedFolderName+ "/" + std::to_string(img_order++) + ".jpg";
        cv::imwrite(img_name, src);
        std::cout<<img_name<<" saved successfully!!!"<<std::endl;
        imgs_sum_now ++;
        interval = tmp_interval;
    }
}

//手动拍照
void UsbCamera::manual_recordImgs()
{
    std::string str = "manualSavedPictures_"+std::to_string(this->get_id())+"_";
    std::string savedFolderName  = Global_Space::setFolderPath(str, 0);
    int img_order = 0;
    std::string img_name;

    std::string window_name = "ManualRecordPictures "+std::to_string(this->get_id());
    cv::namedWindow(window_name,cv::WINDOW_NORMAL);
    cv::Mat src;
    char key;
    std::cout<<"#####  Please Press #####"<<std::endl;
    std::cout<<"# ESC:     exit.        #"<<std::endl;
    std::cout<<"# S/s:     save photo.  #"<<std::endl;
    while (1)
    {
        while (!this->update_img()){;}      //一直取图直到成功
        src = this->src_;
        //可以加上图像处理
        //this->dealImg();
        cv::putText(src, std::to_string(img_order) + ".jpg", cv::Point(30,30),
                    cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,255,0),2);
        cv::imshow(window_name,src);
        key = cv::waitKey(10);

        if(key == 'S' || key == 's')
        {
            //save pictures
            img_name ="./" + savedFolderName+ "/" + std::to_string(img_order++) + ".jpg";
            cv::imwrite(img_name, src);
            std::cout<<img_name<<" saved successfully!!!"<<std::endl;
            this->photo_index ++;
        }

        if(key == 27)
        {
            exit(0);
        }
    }
}

//录制视频
void UsbCamera::recordVideo()
{
    int fps = USB_CAMERA_RECORD_FPS;
    int frame_cnt = 0;

    Restart:
    std::string savedFolderName  = Global_Space::setFolderPath("", 1);
    std::string savedVideoPath = "./" + std::to_string(this->get_id())+"_"+savedFolderName + ".avi";


    cv::Mat src;
    while (!this->update_img()){;}      //一直取图直到成功
    src = this->src_;

    cv::Size size_ = src.size();
    // or you can choose any size
    //cv::Size size_=cv::Size(1280,1024);

    cv::VideoWriter videowriter_(savedVideoPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size_, true);
    if (!videowriter_.isOpened()) {
        std::cout << "videowriter "<< this->get_id()<<" opened failure!" << std::endl;
        exit(0);
    }else
    {
        std::cout << "videowriter "<< this->get_id()<<" opened successfully!!! " << std::endl;
    }

    cv::namedWindow("RecordVideos",cv::WINDOW_NORMAL);
    char key;
    while (1)
    {
        while (!this->update_img()){;}      //一直取图直到成功
        src = this->src_;

        cv::imshow("RecordVideos", src);
        key = cv::waitKey(1);
        if(key == 27)
        {
            exit(0);
        }

        cv::resize(src, src, size_);
        videowriter_ << src;
        frame_cnt ++;
        if((frame_cnt % 15000 )== 0)
        {
            std::cout<<savedVideoPath<<" record finished!!!"<<std::endl;
            videowriter_.release();
            goto Restart;
        }
    }

}


void UsbCamera::show_camera_info()
{
    std::cout<<"##################### USB Camera "<< this->get_id()<<" ##################"<<std::endl;
    std::cout<<"# device path: "<< this->dev_path<<"   | state :"<<Global_Space::enum2str(this->camera_state_)<<" #"<<std::endl;
    std::cout<<"# exposure: "<< this->get_exposure()<<"   | fps: "<< this->get_fps()<<"   | brightness: "<< this->get_brightness()<<"      #"<<std::endl;
    std::cout<<"# resolution width: "<< this->get_resolution_width()<<"   | height: "<< this->get_resolution_height()<<"             #"<<std::endl;
    std::cout<<"# cameraMartix:"<<"  #"<<std::endl;
    std::cout<< this->cameraMartix <<std::endl;
    std::cout<<"# distCoeffs^T:"<<"  #"<<std::endl;
    cv::Mat distcoeffs_transpose;
    cv::transpose(this->distCoeffs,distcoeffs_transpose);
    std::cout<<distcoeffs_transpose<<std::endl;
    std::cout<<"#####################################################"<<std::endl<<std::endl;
}


//根据id获取UsbCamera节点
tinyxml2::XMLElement* UsbCamerasManager::get_UsbCameraNode(tinyxml2::XMLElement*root,const std::string & id)
{
    tinyxml2::XMLElement* node = root->FirstChildElement("UsbCamera");
    while (node != NULL)
    {
        if(node->Attribute("id") == id)
            break;
        else
            node = node->NextSiblingElement();      //下一个兄弟节点
    }
    return node;
}


//载入相机配置xml文件
int UsbCamerasManager::load_cfg_xmlfile()
{
    tinyxml2::XMLError error = this->xmldoc->LoadFile(USB_CAMERA_CFG_PATH);
    if(error != tinyxml2::XMLError::XML_SUCCESS)
    {
        std::cout<<"Error: Load xmlfile "<<USB_CAMERA_CFG_PATH<<" failed."<<std::endl;
        return 0;
    } else
    {
        std::cout<<"Load xmlfile "<<USB_CAMERA_CFG_PATH<<" success."<<std::endl;
    }

    tinyxml2::XMLElement *root = this->xmldoc->RootElement();
    tinyxml2::XMLElement* node = root->FirstChildElement("UsbCamera");
    tinyxml2::XMLElement *device;
    tinyxml2::XMLElement *cameraMatrix;
    tinyxml2::XMLElement *distCoeffs;
    tinyxml2::XMLElement *exposure;
    tinyxml2::XMLElement *fps;
    tinyxml2::XMLElement *resolution_width;
    tinyxml2::XMLElement *resolution_height;
    tinyxml2::XMLElement *brightness;
    int id = 0;
    while (node != NULL)
    {
        device =  node->FirstChildElement("device");
        cameraMatrix = node->FirstChildElement("cameraMatrix");
        distCoeffs = node->FirstChildElement("distCoeffs");
        exposure = node->FirstChildElement("exposure");
        fps = node->FirstChildElement("fps");
        resolution_width = node->FirstChildElement("resolution_width");
        resolution_height = node->FirstChildElement("resolution_height");
        brightness = node->FirstChildElement("brightness");
//        this->usb_cameras[id] = UsbCamera(id+1);                    //此处会执行一次~UsbCamera
        this->usb_cameras[id].set_id(id + 1);
        this->usb_cameras[id].set_dev_path(device->GetText());
        this->usb_cameras[id].set_cameraMartix(cameraMatrix->GetText());
        this->usb_cameras[id].set_distCoeffs(distCoeffs->GetText());
        this->usb_cameras[id].change_camera_params(std::stoi(resolution_width->GetText()),
                                                   std::stoi(resolution_height->GetText()),
                                                   std::stoi(fps->GetText()),
                                                   std::stoi(exposure->GetText()),
                                                   std::stoi(brightness->GetText()));
        this->usb_cameras[id].xml_init = true;
        id++;
        node = node->NextSiblingElement();      //下一个兄弟节点
    }
    return id;
}

UsbCamerasManager::UsbCamerasManager()
{
    /* init manager*/
    xmldoc = new tinyxml2::XMLDocument;
    usb_cameras = new UsbCamera[USB_CAMERA_MAX_SIZE];
    processers = new PROCESS::Processer[USB_CAMERA_MAX_SIZE];
    imgs = new cv::Mat[USB_CAMERA_MAX_SIZE];
    std::memset(this->enable_cameras_id,-1,sizeof(this->enable_cameras_id));
//    usb_cameras_threads = new std::thread[USB_CAMERA_MAX_SIZE];

    /* load configure xml */
    int load_success=this->load_cfg_xmlfile();
    int open_success_count = 0;
    bool open_camera_success;
    if(load_success > 0)
    {
        std::cout<<"xml file has "<<load_success<<" Usb Device Device configure."<<std::endl;
        for(int i = 0; i  < load_success; i++)
        {
            //打开失败则至多重启10次
            if(!this->usb_cameras[i].xml_init)
            {
                this->usb_cameras[i].init_camera_params(640, 480, 30, 15, -10);
            } else{;}

            while(!(open_camera_success= this->usb_cameras[i].open_camera()) && this->usb_cameras[i].reopen_times < USB_CAMERA_MAX_REOPEN_TIMES)
            {
                this->usb_cameras[i].reopen_times++;
                std::cout<<"Reopen usb camera device"<< this->usb_cameras[i].dev_path<<" "<<this->usb_cameras[i].reopen_times<<" times..."<<std::endl;
            }
            if(this->usb_cameras[i].reopen_times == USB_CAMERA_MAX_REOPEN_TIMES)
            {std::cout<<std::endl;}

            //获取启动成功的相机索引
            if(this->usb_cameras[i].reopen_times < USB_CAMERA_MAX_REOPEN_TIMES)
            {
                this->enable_cameras_id[open_success_count] = i+1;

                open_success_count ++;
            }
        }
    } else{
        std::cout<<"xml file has no Usb Camera Device configure."<<std::endl;
    }

    this->threads_count = open_success_count;
};

//录制线程
void UsbCamerasManager::record_thread(const int &id)
{
    Omni_SaveVideo sv(this->usb_cameras[id-1]);
    cv::Mat src;
    while(isrun)
    {
        while((this->usb_cameras[id-1].receive_index - this->usb_cameras[id-1].record_index)<1 && this->isrun)     //阻塞等待取图
        {;}

        this->threads_mutexs[id-1].lock();
        src = this->imgs[id-1].clone();
        this->threads_mutexs[id-1].unlock();
        this->usb_cameras[id-1].record_index++;

        if(src.empty())
        {
            continue;
        }

//        std::cout << "receive_index: " << receive_index << " process_index: " << process_index << " capture_index: " << capture_index << std::endl;
//        double t1 = getTickCount();
        if(sv.getState())
        {
            resize(src,src,sv.size);
            sv.updateImage(src);
        }
//        double t2 = getTickCount();
//        std::cout << (t2 - t1) / getTickFrequency() * 1000 << std::endl;
//        std::cout<<"record thread "<<id<<" running....."<<std::endl;
    }
}

//取图线程
void UsbCamerasManager::receive_thread(const int& id)
{
    while (this->isrun)
    {
        bool grab_success = this->usb_cameras[id-1].update_img();
        if(!grab_success)
            continue;

        if((this->usb_cameras[id-1].receive_index - this->usb_cameras[id-1].process_index)>0 && this->isrun)
            continue;

        this->threads_mutexs[id-1].lock();
        this->imgs[id-1] = this->usb_cameras[id-1].src_;
//        std::cout<<"receive thread "<<id<<" running....."<<std::endl;
        this->threads_mutexs[id-1].unlock();
        this->usb_cameras[id-1].receive_index++;
    }
}

void UsbCamerasManager::receive2_thread(const int& id)
{

    while (this->isrun)
    {
        bool grab_success = this->usb_cameras[id-1].update_img();
        if(!grab_success)
            continue;

        this->threads_mutexs[id-1].lock();
        this->imgs[id-1] = this->usb_cameras[id-1].src_;
        this->threads_mutexs[id-1].unlock();
//        std::cout<<"receive thread "<<id<<" running....."<<std::endl;
        this->usb_cameras[id-1].receive_index++;
    }
}


//处理线程
void UsbCamerasManager::process_thread(const int& id)
{

    while (this->isrun)
    {
        while((this->usb_cameras[id-1].receive_index - this->usb_cameras[id-1].process_index)<1 && this->isrun)     //阻塞等待取图
        {;}

        //图像处理
        this->processers[id-1].processer_ctrl(this->imgs[id-1],id);
//        std::cout<<"process thread "<<id<<" running....."<<std::endl;
        this->usb_cameras[id-1].process_index++;
    }
}

void UsbCamerasManager::process2_thread()
{
    int index = 0;
    cv::Mat src;
    while (this->isrun)
    {
        //如果上一个相机图片还没取完，则直接跳过，处理下一个相机的图片（处理时间比取图快时,可能会一直跳过，好处是不用耗费算力在处理上）
        if((this->usb_cameras[this->enable_cameras_id[index]-1].receive_index - this->usb_cameras[this->enable_cameras_id[index]-1].process_index)<1 && this->isrun)
        {
            index = (index+1) % this->threads_count;
            continue;
        }

        this->threads_mutexs[this->enable_cameras_id[index]-1].lock();
        src = this->imgs[this->enable_cameras_id[index]-1];
        this->threads_mutexs[this->enable_cameras_id[index]-1].unlock();

        //循环处理多个相机的图像
        this->processers[this->enable_cameras_id[index]-1].processer_ctrl(src,this->enable_cameras_id[index]);
//        std::cout<<"process thread "<<this->enable_cameras_id[index]<<" running....."<<std::endl;
        this->usb_cameras[this->enable_cameras_id[index]-1].process_index++;
        index = (index+1) % this->threads_count;

    }
}

bool UsbCamerasManager::exec_usb_cameras_threads()
{
    signal(SIGINT,this->signal_handler);
    std::cout<<"open success count:"<< this->threads_count<<std::endl;

//    for(int i = 0 ; i < USB_CAMERA_MAX_SIZE; i++)
//    {
//        std::cout<< this->enable_cameras_id[i]<<" ";
//    }
//    std::cout<<std::endl;

#ifdef USB_CAMERAS

    #ifdef USB_CAMERA_MUTIPLE_THREADS
        //多取图线程，多处理线程
        if(this->threads_count > 0)
        {
            std::thread receive_threads[this->threads_count];
            std::thread process_threads[this->threads_count];

            //录制-多线程
            #ifdef USB_CAMERA_RECORD_VIDEOS
            Global_Space::createFolderDirectory(USB_CAMERA_VIDEOS_PATH);
            std::thread record_video_threads[this->threads_count];
            #endif

            for(int i = 0; i< this->threads_count; i++)
            {
                receive_threads[i] = std::thread(std::bind(&UsbCamerasManager::receive_thread,this, this->enable_cameras_id[i]));
                process_threads[i] = std::thread(std::bind(&UsbCamerasManager::process_thread, this, this->enable_cameras_id[i]));

                #ifdef USB_CAMERA_RECORD_VIDEOS
                //Warning::不知道会不会出错
                record_video_threads[i] = std::thread(std::bind(&UsbCamerasManager::record_thread,this, this->enable_cameras_id[i]));
                #endif
            }

            for(int i = 0 ; i < this->threads_count; i++)
            {
                if(receive_threads[i].joinable())
                    receive_threads[i].join();
                if(process_threads[i].joinable())
                    process_threads[i].join();

                #ifdef USB_CAMERA_RECORD_VIDEOS
                if(record_video_threads[i].joinable())
                    record_video_threads[i].join();
                #endif
            }

        } else{;}

    #else
        //多线程取图，单线程处理
        if(this->threads_count > 0)
        {
            std::thread receive2_threads[this->threads_count];

            //录制-多线程
            #ifdef USB_CAMERA_RECORD_VIDEOS
            Global_Space::createFolderDirectory(USB_CAMERA_VIDEOS_PATH);
            std::thread record_video_threads[this->threads_count];
            #endif

            for(int i = 0; i < this->threads_count; i++)
            {
                receive2_threads[i] = std::thread(std::bind(&UsbCamerasManager::receive2_thread,this, this->enable_cameras_id[i]));

                #ifdef USB_CAMERA_RECORD_VIDEOS
                //Warning::不知道会不会出错
                record_video_threads[i] = std::thread(std::bind(&UsbCamerasManager::record_thread,this, this->enable_cameras_id[i]));
                #endif
            }
            std::thread process2_thread = std::thread(std::bind(&UsbCamerasManager::process2_thread, this));

            for(int i = 0; i < this->threads_count; i++)
            {
                if(receive2_threads[i].joinable())
                    receive2_threads[i].join();

                #ifdef USB_CAMERA_RECORD_VIDEOS
                                if(record_video_threads[i].joinable())
                                    record_video_threads[i].join();
                #endif
            }
            if(process2_thread.joinable())
                process2_thread.join();

        } else{;}

    #endif


#endif
    return true;
}



//自动拍照
void UsbCamerasManager::exec_auto_photos()
{
    signal(SIGINT,this->signal_handler);
    std::cout<<"open success count:"<< this->threads_count<<std::endl;
    if(this->threads_count>0)
    {
        std::thread auto_photos_threads[this->threads_count];
        for(int i = 0 ; i < this->threads_count; i++)
        {
            auto_photos_threads[i] = std::thread(std::bind(&UsbCamera::auto_recordImgs, this->usb_cameras[this->enable_cameras_id[i] - 1]));
        }
        for(int i = 0 ; i < this->threads_count; i++)
        {
            if(auto_photos_threads[i].joinable())
                auto_photos_threads[i].join();
        }
    } else
    {;}
}

//手动拍照
void UsbCamerasManager::exec_manual_photos()
{
    signal(SIGINT,this->signal_handler);
    std::cout<<"open success count:"<< this->threads_count<<std::endl;
    if(this->threads_count>0)
    {
        std::thread manual_photos_threads[this->threads_count];
        for(int i = 0 ; i < this->threads_count; i++)
        {
            manual_photos_threads[i] = std::thread(std::bind(&UsbCamera::manual_recordImgs, this->usb_cameras[this->enable_cameras_id[i] - 1]));
        }
        for(int i = 0 ; i < this->threads_count; i++)
        {
            if(manual_photos_threads[i].joinable())
                manual_photos_threads[i].join();
        }
    } else
    {;}
}


Omni_SaveVideo::Omni_SaveVideo(const UsbCamera & usb_camera)
{
    this->record_usb_camera = usb_camera;
    this->open_video_writer();
}

Omni_SaveVideo::~Omni_SaveVideo()
{
    video_writer_.release();
    std::cout << "video "<< this->record_usb_camera.get_id()<<" close!!!" << std::endl;
}

void Omni_SaveVideo::open_video_writer(){
    struct tm *newtime;
    char tmpbuf[128];
    time_t test;
    time(&test);
    newtime=localtime(&test);
    strftime(tmpbuf, 128, "%c", newtime);
    sprintf(tmpbuf, "%s.avi", tmpbuf);                      //tmpbuf -> 视频的名称
    // CV_FOURCC('I', 'Y', 'U', 'V')CV_FOURCC('M', 'J', 'P', 'G')
    //    video_writer_.open(tmpbuf, CV_FOURCC('X', 'V', 'I', 'D'), 300, cv::Size(640, 480));

    size=cv::Size(this->record_usb_camera.get_resolution_width(), this->record_usb_camera.get_resolution_height());
    std::cout<<"tmpbuf:"<<tmpbuf<<std::endl;
    //TODO::saved video path
    std::string base =  std::string("./")+USB_CAMERA_VIDEOS_PATH + std::string("/") + std::to_string(this->record_usb_camera.get_id())+"_";

    video_writer_.open(base+tmpbuf, cv::VideoWriter::fourcc('M', 'J', 'P', 'G')  , USB_CAMERA_RECORD_FPS, size);
    if (!video_writer_.isOpened()) {
        std::cout << "videowriter "<< this->record_usb_camera.get_id()<<" opened failure!" << std::endl;
        state_ = false;
    }else
    {
        std::cout << "videowriter "<< this->record_usb_camera.get_id()<<" opened successfully!!! " << std::endl;
        state_ = true;
    }
}

void Omni_SaveVideo::updateImage(cv::Mat &img)
{
    video_writer_ << img;
    frame_count++;
    if(frame_count%(USB_CAMERA_RECORD_MAX_FRAMES)==0){
        video_writer_.release();
        open_video_writer();
    }
}
