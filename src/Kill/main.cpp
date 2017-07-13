#include "../Keep/util.h"

void main()
{
	int ret = KillProcessByName(_T("Keep.exe"));
	if(0 == ret)
	{
		printf("kill success\r\n");
	}
	else if(2 == ret)
	{
		printf("no Keep.exe running\r\n");
	}
	else
	{
		printf("kill failed. %d\r\n", GetLastError());
	}
	system("PAUSE");
}