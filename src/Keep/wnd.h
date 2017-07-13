#ifndef WND_H
#define WND_H


HWND findWindow(HWND parentWnd,const char* name)
{
	HWND hWnd = GetWindow(parentWnd, GW_CHILD); 

	while (hWnd != NULL) 
	{    
		char lpString[32] ={0}; 
		GetWindowText(hWnd, lpString, sizeof(lpString)); 
		if (strstr(lpString,name)) 
		{ 
			return hWnd; 
		} 

		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
	} 
	return NULL; 
}

/************************************************************************/
/* 当前屏幕是不是被锁定了 -- 若取得顶层窗口失败则认为锁定屏幕了
/************************************************************************/
BOOL hasScreenLocked()
{
	HWND hWnd = GetForegroundWindow();
	if(NULL == hWnd)
	{
		return TRUE;
	}
	else return FALSE;
}



void sendKeysWithoutSetForeground(HWND wnd, BYTE * vks, int klen)
{
	HWND focusWnd = SetFocus(wnd); 
	if(focusWnd != wnd)
	{
		//	klog.write("send focus error. code: ");
		//	klog.write(GetLastError());
		//	klog.addLine();
	}

	INPUT *inputs = new INPUT[klen << 1];
	memset(inputs, 0, (klen<<1) * sizeof(INPUT));

	//正序按下
	int i = 0,j = 0;
	for (;i < klen;++ i, ++ j)
	{
		//按下 向下方向键
		inputs[j].ki.wVk = *(vks + i);
		inputs[j].type = INPUT_KEYBOARD;
	}

	//反序松开
	for (-- i;i>=0;-- i, ++ j)
	{
		//松开 向下方向键
		inputs[j].ki.wVk = *(vks + i);
		inputs[j].type = INPUT_KEYBOARD;
		inputs[j].ki.dwFlags = KEYEVENTF_KEYUP;
	}

	assert(j == (klen << 1));

	//该函数合成键盘事件和鼠标事件，用来模拟鼠标或者键盘操作。事件将被插入在鼠标或者键盘处理队列里面
	SendInput(klen << 1, inputs, sizeof(INPUT));

	delete[] inputs;
}


void sendToForegroundWnd(BYTE* vks, int klen)
{
	HWND hWnd = GetForegroundWindow();
	if(NULL == hWnd)
	{
		LOG_INFO("can't execute.");
		return;
	}

	TCHAR text[256] = {0};
	int tlen = GetWindowText(hWnd,text,255);
	TCHAR buffer[1024] = {0};
	sprintf_s(buffer,1023,"current foreground window is : %s",text);
//	LOG_INFO(buffer);

	sendKeysWithoutSetForeground(hWnd, vks, klen);
	LOG_INFO("finished.");
}

#endif