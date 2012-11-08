// getSystemInfo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")


int _tmain(int argc, _TCHAR* argv[])
{
	SYSTEM_INFO siSysInfo;

	// Copy the hardware information to the SYSTEM_INFO structure. 

	GetSystemInfo(&siSysInfo); 

	// Display the contents of the SYSTEM_INFO structure. 

	printf("Hardware information: \n"); 
	printf("Process Architecture is ");
	if (siSysInfo.dwOemId == PROCESSOR_ARCHITECTURE_INTEL)
		printf("x86\n");
	else if (siSysInfo.dwOemId == PROCESSOR_ARCHITECTURE_AMD64)
		printf("x64 (AMD or Intel)\n");
	else if (siSysInfo.dwOemId == PROCESSOR_ARCHITECTURE_ARM)
		printf("ARM\n");
	else if (siSysInfo.dwOemId == PROCESSOR_ARCHITECTURE_IA64)
		printf("Intel Itanium-based\n");
	else if (siSysInfo.dwOemId == PROCESSOR_ARCHITECTURE_IA64)
		printf("PROCESSOR_ARCHITECTURE_UNKNOWN\n");
	//printf("OEM ID: %u\n", siSysInfo.dwOemId);
	printf("Number of processors: %u\n", siSysInfo.dwNumberOfProcessors); 
	printf("Page size: %u\n", siSysInfo.dwPageSize); 
	printf("Processor type: %u\n", siSysInfo.dwProcessorType); 
	printf("Minimum application address: %lx\n", siSysInfo.lpMinimumApplicationAddress); 
	printf("Maximum application address: %lx\n", siSysInfo.lpMaximumApplicationAddress); 
	printf("Active processor mask: %u\n", siSysInfo.dwActiveProcessorMask); 
	return 0;
}

