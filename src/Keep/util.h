#ifndef UTIL_H
#define UTIL_H
#include <fstream>
using namespace std;
#include <TCHAR.H>

#include <windows.h>
#include <process.h>
#include <tlhelp32.h>


int lastSlashPos(const TCHAR* inStr)
{
	int lastSlashPos = 0;
	int i = 0;
	TCHAR c = 0;
	while (0 != (c = *(inStr + i)))
	{
		if ('/' == c || '\\' == c)
		{
			lastSlashPos = i;
		}
		++ i;
	}
	return lastSlashPos;
}

#define NEED_DELETE 

const TCHAR* NEED_DELETE getCurrentModulePathWithSlash()
{
	TCHAR moduleFile[256]  = {0};
	GetModuleFileName(NULL,moduleFile,255);
	int length = lastSlashPos(moduleFile) + 1;
	
	TCHAR *ret = new TCHAR[length + 1];
	//memset(ret,0,(length + 1) * sizeof(TCHAR));
	ret[length] = 0;
	memcpy_s(ret,length,moduleFile,length);
	return ret;
}

int fileExists(const TCHAR* filename)
{
	fstream file;
	file.open(filename,ios::in);
	if(!file)	//与 file.failed() 等价
	{
		return 0;
	}
	else
	{
		file.close();
		return 1;
	}
}


#pragma data_seg("Shared") 
int volatile g_lAppInstance = 0; 
#pragma data_seg() 
#pragma comment(linker,"/section:Shared,RWS")
int tryFirstRunModule()
{
	if (0 == g_lAppInstance)
	{
		g_lAppInstance = 1;
		return 0; //以前没有运行过
	}
	else 
	{
		return 1;
	}
}


BOOL KillProcess(DWORD dwPid, DWORD waitKillMs)  
{  
	HANDLE hPrc;  

	if( 0 == dwPid) return FALSE;  

	hPrc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPid);  // Opens handle to the process.  

	if( !TerminateProcess(hPrc,0) ) // Terminates a process.  
	{  
		CloseHandle( hPrc );  
		return FALSE;  
	}  
	else  
		WaitForSingleObject(hPrc, waitKillMs); // At most ,waite 2000  millisecond.  

	CloseHandle(hPrc);  
	return TRUE;  
}  


BOOL EnableDebugPrevilige(BOOL   fEnable) 
{ 
	BOOL   fOk   =   FALSE;         //   Assume   function   fails 
	HANDLE   hToken;
	if   (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,   
		&hToken))
	{
		TOKEN_PRIVILEGES   tp; 
		tp.PrivilegeCount   =   1; 
		LookupPrivilegeValue(NULL,   SE_DEBUG_NAME,   &tp.Privileges[0].Luid); 
		tp.Privileges[0].Attributes   =   fEnable   ?   SE_PRIVILEGE_ENABLED   :   0; 
		AdjustTokenPrivileges(hToken,   FALSE,   &tp,   sizeof(tp),   NULL,   NULL); 
		fOk   =   (GetLastError()   ==   ERROR_SUCCESS); 
		CloseHandle(hToken); 
	} 
	return(fOk);
}

/************************************************************************/
/*  返回 2 表示没找到进程
	返回 1 表示关闭进程失败
	返回 0 表示关闭成功
/************************************************************************/
int KillProcessByName(const TCHAR *lpszProcessName) 
{  
	EnableDebugPrevilige(TRUE);
	unsigned int   pid = -1;  
	int    retval = 2;  

	if (lpszProcessName == NULL)  
		return -1;

	DWORD dwRet = 0;  
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS,0 );  
	PROCESSENTRY32 processInfo;  
	processInfo.dwSize = sizeof( PROCESSENTRY32 );  
	int flag = Process32First( hSnapshot, &processInfo );  

	// Find the process with name as same as lpszProcessName  
	while (flag != 0)  
	{  
		if (_tcscmp(processInfo.szExeFile, lpszProcessName) == 0) {  
			retval = 1;
			// Terminate the process.  
			pid = processInfo.th32ProcessID;  
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);  

			if (TerminateProcess(hProcess, 0) != TRUE) { // Failed to terminate it.  
				break;  
			}
			else
			{
				retval = 0;	//关闭成功
			}
		}  

		flag = Process32Next(hSnapshot, &processInfo);   
	} // while (flag != 0)  

	CloseHandle(hSnapshot);  
	EnableDebugPrevilige(FALSE);

	return retval;  
}


#endif