#pragma once

#include <atlstr.h>

class CDynamicPackage 
{
public:
	CDynamicPackage();
	~CDynamicPackage();

public:
	BOOL Init();

protected:
	CString		m_strSignToolPath;			// 签名工具与打包程序相对路径
	CString		m_strCrossCertifyPath;	// 交叉签名证书与打包程序相对路径
	CString		m_strScriptPath;				// 编译脚本与打包程序相对路径

	static CString m_strMakeNsisPath;	//	编译程序安装目录
};