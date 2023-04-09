
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <psapi.h>
#include <wchar.h>
#include <string.h>
#include <TlHelp32.h>
#include <process.h>


int* pointer_path;
int num_ptr;


uintptr_t GetProcessBaseAddress(DWORD processId)
{

	uintptr_t   baseAddress = 0; 
	HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processId); 
	HMODULE*    moduleArray;
	LPBYTE      moduleArrayBytes;
	DWORD       bytesRequired;

	if (processHandle)
	{
		if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
		{
			if (bytesRequired)
			{
				moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

				if (moduleArrayBytes)
				{
					int moduleCount;

					Sleep(1 * 50);

					moduleCount = bytesRequired / sizeof(HMODULE);
					moduleArray = (HMODULE*)moduleArrayBytes;


					if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
					{
						baseAddress = (uintptr_t)(moduleArray[0]);
					}

					LocalFree(moduleArrayBytes);
				}
			}
		}

		CloseHandle(processHandle);
	}

	return baseAddress;
}
DWORD GetProcId(WCHAR* name)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (wcscmp(entry.szExeFile, name) == 0)
			{
				return entry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshot);
	return 0;
}

int main(int argc, char* argv[])
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	WinExec("mcdata/Minecraft.Windows.exe", SW_SHOW);
	int LOGIN_STEP_VALUE = -1;

	{
		num_ptr = 8;
		pointer_path = (int*)malloc(num_ptr * sizeof(int));
		printf_s("Failed, using default pointer path (MCEE 1.18.31)\n");
		pointer_path[0] = 0x03AAD164;
		pointer_path[1] = 0x0;
		pointer_path[2] = 0x55C;
		pointer_path[3] = 0x16C;
		pointer_path[4] = 0x4C;
		pointer_path[5] = 0x5C;
		pointer_path[6] = 0x38;
		pointer_path[7] = 0x14;
	}


	printf_s("\nPointer Path: "); 
	for (int i = 0; i < num_ptr; i++) 
	{
		printf_s("%x", pointer_path[i]); 
		if (i != num_ptr - 1) 
		{
			printf_s(" > "); 
		}
	}
	printf_s("\n"); 

	DWORD proc_id = 0;
	while (proc_id == 0)
	{
		proc_id = GetProcId(L"Minecraft.Windows.exe"); 
		if (proc_id == 0)
		return 0;
	}
	printf_s("MCEE Process ID: %x\n", proc_id);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, proc_id);
	printf_s("MCEE Process Handle: 0x%p\n", hProcess);
	
	{
		uintptr_t baseAddress = 0;
		while (baseAddress == 0)
			baseAddress = GetProcessBaseAddress(proc_id); 

		printf_s("MCEE Base Addr: 0x%p\n", (void*)baseAddress);
		printf_s("Waiting for game to initalize....\n");

		

	read_ptr_path:

		baseAddress = GetProcessBaseAddress(proc_id); 
		
		uintptr_t first_ptr = pointer_path[0];
		uintptr_t cur_ptr = baseAddress + first_ptr;
		uintptr_t ptr = 0;
		uintptr_t new_ptr = 0;

		ReadProcessMemory(hProcess, (LPCVOID)cur_ptr, &ptr, sizeof(uintptr_t), 0);
		if (ptr == 0)
			goto read_ptr_path;
		
		for (int i = 1; i < num_ptr-1; i++)
		{
			cur_ptr = ptr + pointer_path[i];
			ReadProcessMemory(hProcess, (LPCVOID)cur_ptr, &new_ptr, sizeof(uintptr_t), 0);
			if (new_ptr == 0) {
				i -= 1;
				goto read_ptr_path;
			}
			else
			{
				ptr = new_ptr;
				
			}
		}

		ptr += pointer_path[num_ptr-1]; 

		int login_step_value = 0;
		ReadProcessMemory(hProcess, (LPCVOID)ptr, &login_step_value, sizeof(int), 0);

		if (login_step_value != 0x0)
		{

			printf_s("Final Ptr: 0x%p\n", (void*)ptr);

			printf_s("Current Login Step: %i\n", login_step_value);
			if (LOGIN_STEP_VALUE != -1)
			{
				printf_s("Trying login stage %i", LOGIN_STEP_VALUE);
				WriteProcessMemory(hProcess, (LPVOID)ptr, &LOGIN_STEP_VALUE, sizeof(int), 0);
			}

			Sleep(1 * 250);

			printf_s("Trying login stage 9...\n");
			login_step_value = 9;
			WriteProcessMemory(hProcess, (LPVOID)ptr, &login_step_value, sizeof(int), 0);

		}
		else
			goto read_ptr_path;

		CloseHandle(hProcess);

		return 0;
	}
}
