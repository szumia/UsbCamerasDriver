# USB相机驱动代码
支持以下功能：  
* 启动单个相机或无穷多个相机，只需要编写"config"中的xml配置文件即可
* 打开“defines”中的相应宏，并更改相应值，实现多种功能
* 多线程取图+多线程处理
* 多线程取图+单线程处理
* 多线程录制视频并自动保存
* 多线程自动拍照并自动保存
* 多线程手动拍照并自动保存
* 扩展性：在"process_img"中实现你的代码，实现个性化功能  

系统支持： Linux  
硬件支持： 外接USB摄像头  
环境支持： c++17、opencv5.6+、tinyxml2


快速上手：
* 编辑配置文件[usb_camera_cfg.xml](config%2Fusb_camera_cfg.xml)
* 编辑rulse文件[usb.rules](config%2Fusb.rules)并导入你的系统中
* $cd cmake-build-debug
* $./PlayUsbCameras

### USB摄像头驱动相关

[多usb摄像头串口设备名固定和永久授权](https://blog.csdn.net/weixin_44023934/article/details/122252740)

[c++使用tinyxml2库解析xml文件](https://www.jianshu.com/p/432d62f80686)

[usb相机参数设置命令](https://github.com/szumia/AssistFolders/blob/main/usb%E7%9B%B8%E6%9C%BA%E5%8F%82%E6%95%B0%E8%AE%BE%E7%BD%AE%E5%91%BD%E4%BB%A4.txt)

[同型号多个USB相机固定设备号](https://blog.csdn.net/qq_30460905/article/details/106038869)

* 拔插查看设备号,主要是获取idVendor和idProduct（相同厂家相同型号的相机两个id号是相同的）   
```$lsusb```
* 拔插查看连接的相机名，一般一个相机对应2个号如video0 video1，使用时使用偶数即video0   
```$ls /dev/video*```
* 查看相机设备详细信息，找到对应idVendor和idProduct处的KERNELS值  
```$udevadm info -a /dev/video0 ```
* 将KERNEL =="video[0,2,4,6,8]*",KERNELS == "3-6.3",ATTRS{idVendor}== "0edc",ATTRS{idProduct}=="2050",GROUP="video",MODE="0777",SYMLINK+= "video_cam_01" 写入rules文件
* KERNELS值不同可以固定相同厂家相同型号相机，idVendor和idProduct对应厂家和型号，SYMLINK是自定义名称
* 将rules文件放入系统
```$cp xxx.rules /etc/udev/rules.d```
* 激活rules后，重新拔插设备即可   
```$sudo udevadm control --reload```  
```$sudo service udev restart```  
* 使用v4l查看并更改模组相机参数  
```安装v4l          $sudo apt install v4l-utils```  
```查看能修改的参数   $v4l2-ctl -d /dev/video0 --list-ctrls```  
```关闭自动曝光      $v4l2-ctl -d /dev/video0 -c auto_exposure=1```  

#### 注意事项
如果将几个相同型号USB相机插在同一拓展坞，有时会导致某个相机取图错误，有时会串相机。
尽量使用原生接口接入USB相机设备。



### Author
Thank you for your support and star. Your insightful suggestions will be adopted.  
If you have any willingness to communicate, please contact 13046311074@163.com
