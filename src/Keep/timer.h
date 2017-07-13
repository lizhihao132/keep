#ifndef TIMER_H
#define TIMER_H

#include <windows.h>
#include "kLog.h"

class KLog;
extern KLog klog;

typedef void (* TIMER_CALL_BACK_PROC)(void *);

void timeRun(TIMER_CALL_BACK_PROC proc, void *lparam, int intervalSecs)
{
	HANDLE keepTimer = CreateWaitableTimerEx(NULL,_T("keep_timer"),CREATE_WAITABLE_TIMER_MANUAL_RESET,EVENT_ALL_ACCESS);
	if(NULL == keepTimer)
	{
		LOG_ERROR("create timer error. ", GetLastError());
		return ;
	}
	if(ERROR_ALREADY_EXISTS == GetLastError())
	{
		LOG_INFO("timer already exists.");
	}
	LARGE_INTEGER liDueTime;//从何时开始有信号
	liDueTime.QuadPart = ((LONGLONG)-10000000) * intervalSecs;	//相对时间，在什么时候之后触发. 100 纳秒为基
	//回调函数为空，后面使用 waitForSingleObject 去等待，则回调函数会被忽略. 最后一个参数指示了是否将系统从电源保护模式中恢复过来

	while (true)
	{
		if(0 == SetWaitableTimer(keepTimer,&liDueTime,0,NULL,NULL,FALSE))
		{
			LOG_ERROR("set timer error. ", GetLastError());
			return ;
		}

		if(WAIT_OBJECT_0 != WaitForSingleObject(keepTimer,INFINITE))
		{
			LOG_ERROR("wait timer error. ", GetLastError());
			return ;
		}
		try
		{
			proc(lparam);
		}
		catch (... )
		{
			LOG_ERROR("timer proc exception.", 0);
		}
		
	}
}

#endif