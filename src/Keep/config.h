#ifndef CONFIG_H
#define CONFIG_H
#include <windows.h>

class Config
{
public:
	Config(const TCHAR* configFile):configFile(configFile){

	}
	UINT getAsUint(const TCHAR* secion, const TCHAR* key, UINT defaultValue)
	{
		return GetPrivateProfileInt(secion,key,defaultValue,configFile);
	}
protected:
private:
	const TCHAR* configFile;
};

#endif