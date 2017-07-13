#ifndef CONTROL_H
#define CONTROL_H

#include "wnd.h"
#include "lastinput.h"
#include "kLog.h"

class Control
{
public:
	Control(DWORD targetScreenTimeoutSecs_, DWORD sysScreenTimeoutSecs_)
		:targetScreenTimeoutSecs(targetScreenTimeoutSecs_),sysScreenTimeoutSecs(sysScreenTimeoutSecs_)
	{		
		userLastInputTickcountMs = lastInputService.getValatile();
		myLastInputTickcountMs = -1;
	}

	void run()
	{
		if(0 != this->lockScreenInNeed())
			return;
		if(0 != this->keepScreenInNeed())
			return;
	}
protected:
	DWORD calUserTimeout()
	{
		DWORD sinceSystemOnMs = GetTickCount();	//系统已经运行的时间段
		DWORD curInputTickcountMs = lastInputService.getValatile();
		//若当前获得的"最后输入时间"不是本程序保存的最后输入时间,则说明当前获得的"最后输入时间"指的是用户的输入.
		//否则指的是模拟输入
		if(myLastInputTickcountMs != curInputTickcountMs)
		{
			//当前的输入是用户输入
			DWORD timeoutMs = sinceSystemOnMs - curInputTickcountMs;
			userLastInputTickcountMs = curInputTickcountMs;
			return timeoutMs;
		}
		else
		{
			//当前的输入是模拟输入.
			return sinceSystemOnMs - userLastInputTickcountMs;
		}
	}

	int doLockWorkStation()
	{
		return LockWorkStation();
	}

	int lockScreenInNeed()
	{
		DWORD userTimeoutMs = calUserTimeout();
		if(userTimeoutMs >= (1000 * this->targetScreenTimeoutSecs))
		{
			if (hasScreenLocked())
			{
				return 1;
			}
			LOG_INFO("-------------------------------------");
			TCHAR buffer[1024] = {0};
			sprintf_s(buffer,1023,"user last input time : %d", userTimeoutMs);
			LOG_INFO(buffer);
			if(!doLockWorkStation())
				LOG_ERROR("LockWorkStation failed with %d\n", GetLastError());
			else
				LOG_INFO("LockWorkStation success");
			return 1;
		}

		return 0;
	}

	int keepScreenInNeed()
	{
		if(hasScreenLocked())
		{
			return 0;
		}
		DWORD sinceSystemOnMs = GetTickCount();	//系统已经运行的时间段
		DWORD curInputTickcountMs = lastInputService.getValatile();

		if (sinceSystemOnMs - curInputTickcountMs >= (1000 * (sysScreenTimeoutSecs >> 1)))
		{
			LOG_INFO("-------------------------------------");
			TCHAR buffer[1024] = {0};
			sprintf_s(buffer,1023,"keep it. time out : %d ms, compare(sysScreenTimeoutSecs/2) : %d ms", (sinceSystemOnMs-curInputTickcountMs),1000 * (sysScreenTimeoutSecs >> 1));
			LOG_INFO(buffer);
			try
			{
				BYTE vks[] = {VK_CONTROL,VK_MULTIPLY};	// ctrl + *
				sendToForegroundWnd(vks, sizeof(vks));
				myLastInputTickcountMs = sinceSystemOnMs;
				return 1;
			}
			catch (...)
			{
				LOG_INFO("opps, exception.");
			}
		}
		return 0;
	}
	

public:
	const DWORD targetScreenTimeoutSecs;
	const DWORD sysScreenTimeoutSecs;
	DWORD myLastInputTickcountMs;
	DWORD userTimeoutMs;	//客户超时时间: 客户最后一次操作的时间到上一次操作时间(可能是本程序的模拟操作)
	DWORD userLastInputTickcountMs;	//系统上一次的最后输入时间
};


void timerProc(void *lparam)
{
	Control * control = (Control*) lparam;
	
	control->run();
}

void testScreenLocked(void *lparam)
{
	if(hasScreenLocked())
	{
		LOG_INFO("screen has locked.");
	}
	else
	{
		LOG_INFO("screen has NOT locked.");
	}
}


#endif