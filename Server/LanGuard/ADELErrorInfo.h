#pragma once

// 指针初始化失败，一般是由于动态库加载失败
#define INVALID_PTR_VALUE		((INT)(LONG)-1)
// 客户端传输内容中版本不正确
#define ERROR_VERSION				((INT)(LONG)-2)
// 客户端控制命令不正确
#define ERROR_CMD						((INT)(LONG)-3)

CString GetADELErrorInfo(int nErrorCode);