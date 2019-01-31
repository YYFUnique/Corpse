#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

template<typename T> 
class DLL_API CRemoteProcess
{
public:
	CRemoteProcess(DWORD dwProcessId = 0, DWORD dwDesiredAccess = PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE ,
		DWORD flAllocationType = MEM_COMMIT, DWORD flProtect = PAGE_READWRITE)
	{
		m_hProcess = OpenProcess(dwDesiredAccess, FALSE, dwProcessId ? dwProcessId : GetCurrentProcessId());
		if (m_hProcess==NULL)
			return;

		ASSERT(m_hProcess);
		if(m_hProcess)
		{
			m_lpData = VirtualAllocEx(m_hProcess, NULL, sizeof(T), flAllocationType, flProtect);
			ASSERT(m_lpData);
		}
	}

	~CRemoteProcess()
	{
		if (m_hProcess)
		{			
			if (m_lpData)
			{
				VirtualFreeEx(m_hProcess, m_lpData, NULL, MEM_RELEASE);
				m_lpData = NULL;
			}
			CloseHandle(m_hProcess);
		}
	}

	//WriteData is used to copy data to memory in the foreign process
	BOOL WriteData(const T& data)
	{
		return (m_hProcess && m_lpData) ? WriteProcessMemory(m_hProcess, m_lpData, 
			(LPCVOID)&data, sizeof(T), NULL) : FALSE;
	}

	//ReadData reads back data from memory in the foreign process
	BOOL ReadData(T* data)
	{
		return (m_hProcess && m_lpData) ? ReadProcessMemory(m_hProcess, m_lpData, 
			(LPVOID)data, sizeof(T), NULL) : FALSE;
	}

	//Templated ReadData that's used to read a specific data type from
	//a memory address located in the foreign process
	template<typename TSUBTYPE> BOOL ReadData(TSUBTYPE* data, LPCVOID lpData)
	{
		return m_hProcess ? ReadProcessMemory(m_hProcess, lpData, 
			(LPVOID)data, sizeof(TSUBTYPE), NULL) : FALSE;
	}

	template<typename TSUBTYPE,SIZE_T nElement>BOOL ReadData(TSUBTYPE* pData,LPCVOID lpData)
	{
		return m_hProcess ? ReadProcessMemory(m_hProcess, lpData,
			(LPVOID)pData , sizeof(TSUBTYPE) * nElement , NULL) : FALSE;
	}

	//Gets the address of the allocated memory in the foreign process
	const T* GetData()
	{
		return (m_hProcess && m_lpData) ? (T*)m_lpData : NULL;
	}
private:
	HANDLE m_hProcess;
	LPVOID m_lpData;
};