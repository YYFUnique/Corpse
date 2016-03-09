#ifndef GlobalInfo_h__
#define GlobalInfo_h__

typedef enum tgaCAMERA_INFO
{
	NoCamera = 1,			//没有摄像头设备，默认值
	NoCameraDriver,		//没有安装摄像头驱动
	InstallCamera,			//有摄像头设备并且安装了驱动
}CAMERA_INFO;

#endif // GlobalInfo_h__