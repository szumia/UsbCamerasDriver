//
// Created by lzt on 24-1-18.
// 公用函数
//

#ifndef GLOBAL_SPACE_HPP
#define GLOBAL_SPACE_HPP

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>

namespace Global_Space{

   inline std::string enum2str(int value){
        std::string result;
        switch (value) {
            case 0:
                result = "IDLE";
                break;
            case 1:
                result = "RUNNING";
                break;
            case 2:
                result = "ERROR";
                break;
            default:
                result = "ILLEGAL";
                break;

        }
        return result;
    }


    /*
     * 创建文件夹
     * type 文件类型  0 - 照片  1 - 视频（不创建文件夹，只返回时间字符串）
     */
  inline  std::string setFolderPath(std::string foldername, int type)
    {
        std::stringstream ss;
        auto systemtime = std::chrono::system_clock::now();
        time_t  systemtime_now =std::chrono::system_clock::to_time_t(systemtime);
        tm* systemtime_local = localtime(&systemtime_now);
        ss<<systemtime_local->tm_year + 1900 <<"_"
          <<systemtime_local->tm_mon + 1 <<"_"
          <<systemtime_local->tm_mday <<"_"
          <<systemtime_local->tm_hour <<"_"
          <<systemtime_local->tm_min <<"_"
          <<systemtime_local->tm_sec;

        std::string savedFolderName  = foldername + ss.str();    //保存的路径

        if(type == 0)
        {
            //c++17才能使用filesystem
            std::filesystem::path folderpath = savedFolderName;
            if(!std::filesystem::exists(folderpath))
            {
                std::filesystem::create_directory(folderpath);
                std::cout<<savedFolderName<<" created successfully!"<<std::endl;
            } else
            {
                std::cout<<savedFolderName<<" already exists!"<<std::endl;
            }
        } else{;}

        return savedFolderName;
    }

    //创建文件夹
    inline bool createFolderDirectory(std::string folder_name)
    {
        std::filesystem::path folderpath = folder_name;
        if(!std::filesystem::exists(folderpath))
        {
            std::filesystem::create_directory(folderpath);
            std::cout<<"folder: "<<folder_name<<" created successfully!"<<std::endl;
        } else{
            std::cout<<"folder: "<<folder_name<<" already exists!"<<std::endl;
        }
    }

}


#endif //GLOBAL_SPACE_HPP
