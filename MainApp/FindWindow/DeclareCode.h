#ifndef DECLARE_CODE_H
#define DECLARE_CODE_H

/*=======链接状态描述=========*/
#define LINK_NONE	    0x1		//只是文本
#define LINK_URL			0x2		//URL超连接
#define LINK_MAIL		0x3		//邮箱地址
#define LINK_PIC			0x4		//图像连接，如果不设置连接地址就仅仅显示图片
#define LINK_CHILD		0x5		//向父对话框发送点击消息

/*======静态文本控件状态======*/
#define LINK_NORMAL     0x0000L   //平时显示状态
#define LINK_VISITED		0x0001L	//链接访问后
#define LINK_HOVER		0x0002L	//鼠标移动到控件上
#define LINK_CLICK			0x0003L	//鼠标点击后

#endif // DECLARE_CODE_H