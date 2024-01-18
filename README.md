# USB相机驱动代码
支持以下功能：  
* 启动单个相机和无穷多个相机，只需要编写"config"中的xml配置文件即可
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

### 相关链接
Linux驱动多个USB相机，需要授权和固定设备号。部分USB相机还需要关闭自动曝光才可调节曝光。
[多usb摄像头串口设备名固定和永久授权](https://blog.csdn.net/weixin_44023934/article/details/122252740)

[c++使用tinyxml2库解析xml文件](https://www.jianshu.com/p/432d62f80686)

[usb相机参数设置命令](https://github.com/szumia/AssistFolders/blob/main/usb%E7%9B%B8%E6%9C%BA%E5%8F%82%E6%95%B0%E8%AE%BE%E7%BD%AE%E5%91%BD%E4%BB%A4.txt)


### Author
Thank you for your support and star. Your insightful suggestions will be adopted.  
If you have any willingness to communicate, please contact 13046311074@163.com
