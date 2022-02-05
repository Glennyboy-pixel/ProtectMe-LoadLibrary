#include <windows.h>
#include <winternl.h>
#include <conio.h>
#include "LdrInstances.h"

BOOL __stdcall ModifiedEntryPoint(HINSTANCE ModuleInstance, DWORD Reason, LPVOID lpReserved)
{
	if (Reason == DLL_PROCESS_ATTACH)
	{
		MessageBoxA(0, "DLL loading", 0, 0);
	}
	else if (Reason == DLL_PROCESS_DETACH)
	{
		MessageBoxA(0, "DLL exiting", 0, 0);
	}

	return FALSE;
}

BOOL WINAPI _LdrDllHandler(
	_In_ PLDR_DATA_TABLE_ENTRY2 Module,
	_In_ BOOL bIsLoading
)
{
	if (bIsLoading)
	{
		printf("Loading: %ws\n", Module->BaseDllName.Buffer);

		if (StrCmpW(Module->BaseDllName.Buffer, L"mfc120.dll") == 0)
		{
			Module->LoadCount = 1;
			Module->EntryPoint = &ModifiedEntryPoint;
		}
	}
	else
	{
		printf("Unloading: %ws\n", Module->BaseDllName.Buffer);
	}

	return TRUE;
}

int __cdecl main()
{
	LdrInstance ldr;

	HMODULE hModuleBase = NULL;
	PWSTR swModuleName = L"mfc120.dll";

	if (!ldr.Initialize())
	{
		printf("Failed to initialized.\n");
		ExitProcess(0);
	}

	ldr.RegisterWatchContext(_LdrDllHandler); 

	hModuleBase = LoadLibraryW(swModuleName); //will be unloaded by context
	
	printf("Module %S loaded in: 0x%llx\n", swModuleName, hModuleBase);

	Sleep(INFINITE);

	return 0;
}