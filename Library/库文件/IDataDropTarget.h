#ifndef __UI_DROPTARGET__H
#define __UI_DROPTARGET__H

#pragma once

#include <shobjidl.h>
#include <ShlGuid.h>

namespace DuiLib
{
	// {2B940F60-14B9-49b9-B241-83BD0EE1B9B2}
	_declspec(selectany) IID IID_IIDataDropTarget = { 0x2b940f60, 0x14b9, 0x49b9, { 0xb2, 0x41, 0x83, 0xbd, 0xe, 0xe1, 0xb9, 0xb2 } };
	/************************************************************************/
	/* ע�⣺����DragDropRegister������                                                      */
	/*			   ��WinMain������ʼ��ʱ��ֻ��ʹ��OleInitialize��ʼ�����м�	   */
	/************************************************************************/
	class UILIB_API IDataDropTarget : public IDropTarget
	{
	public:  
		IDataDropTarget(void);  
		bool DragDropRegister(HWND hWnd,DWORD AcceptKeyState=MK_LBUTTON);  
		bool DragDropRevoke();

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, __RPC__deref_out void **ppvObject);  
		ULONG STDMETHODCALLTYPE AddRef();  
		ULONG STDMETHODCALLTYPE Release();  
		//�Ϸ�Դ����Ŀ�괰��
		HRESULT STDMETHODCALLTYPE DragEnter(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);  
		//�Ϸ�Դ�ڴ������ƶ�
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);  
		//�Ϸ�Դû�н����ݶ�����ڴ��ڣ����뿪��
		HRESULT STDMETHODCALLTYPE DragLeave();  
		//�Ϸ�Դ�����������˴���
		HRESULT STDMETHODCALLTYPE Drop(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);  
	private:  
		~IDataDropTarget(void);  

		HWND m_hWnd;  
		IDropTargetHelper* m_piDropHelper;  
		bool    m_bUseDnDHelper;  
		DWORD m_dAcceptKeyState;  
		LONG  m_lRefCount;  
	};
}

#endif //__UI_DROPTARGET__H