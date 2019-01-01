# FFMPEG_And_Other_Study
关于FFmpeg和OpenCV的实践，相关项目及其涉及到的流媒体相关技术备忘
<br>
相关代码及资源来自于夏曹俊老师的学习参考代码,来源网站：http://www.ffmpeg.club/

## 开发环境：
    WIN7 64位 Visual Studio 2015
    QT 5.9 32位 
    FFmpeg 3.3 32位
    Opencv 3.2 32位	
	bin目录下有可执行程序需要的dll文件，需要根据库版本添加对应的dll文件。 

## FFMPEG_Practice子项目
  	关于FFMPEG相关的测试与练习代码。
	1.使用文件推流到RTMP
## OpenCV_Practice子项目
	关于OpenCV相关的测试与练习代码。
	1.简单测试显示图片
	2.简单测试Mat对象
	3.简单测试获取USB摄像头和网络摄像头的数据
	4.使用OpenCV做rtsp to rtmp，并添加异常处理
	5.初始化像素格式上下文
	6.rtsp数据源到rtmp推流   重点
	7.opencv_rtsp_to_rtmp_class封装重构代码   重点 
## Qt_Audio_Rtmp子项目
	关于在QT库中结合FFMPEG实现音频推流相关的测试代码。
	1.QT录制音频测试
	2.音频推流
	3.音频推流代码重构
## Qt_Audio_Rtmp_Class子项目
	关于在QT库中结合FFMPEG实现音频推流相关的测试代码的类封装。
	1.利用封装的录制音频的接口，录制缓冲队列处理
## Audio_And_video_Synchronization子项目
	关于在QT库中结合FFMPEG、OpenCV实现摄像头音视频的获取及图像美颜并推流。
## QTRtmpStreamer子项目
	流媒体直播的具体实现
