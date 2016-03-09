#include <windows.h>
#include "NonRegularWindowDlg.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CNonRegularWindowDlg NonRegularWindow;
	NonRegularWindow.Show();
}