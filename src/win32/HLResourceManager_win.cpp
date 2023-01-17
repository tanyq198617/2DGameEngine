#include "HLResourceManager.h"
#include <windows.h>

NS_HL_BEGIN

void HLResourceManager::initResourcePathsNative()
{
	char currPath[MAX_PATH] = { 0 };
	WCHAR  wszPath[MAX_PATH] = { 0 };
	int nNum = WideCharToMultiByte(CP_ACP, 0, wszPath,
		GetCurrentDirectoryW(sizeof(wszPath), wszPath),
		currPath, MAX_PATH, NULL, NULL);
	char *t = currPath;
	while (*t)
	{
		if (*t == '\\')
		{
			*t = '/';
		}

		++t;
	}
	strcat(currPath, "/Resources");
	addFileSearchPath(currPath);

	mWritablePath = currPath;
	mWritablePath += "/";
}

HLFileData* HLResourceManager::getFileDataInAppBundle(const char* file)
{
	return NULL;
}

bool HLResourceManager::fileInAppBundle(const char* file)
{
	return false;
}

NS_HL_END