//LUA REF
//https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/485402-createremotethread-how-pass-not-ptr-argument.html
//https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/518567-safe-use-lua-dostring-function-now.html
//https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/401212-lua_dostring-c-w-createremotethread-writeprocessmemory-security-questions.html

//LUA DOSTRING
//https://vanilla-wow-archive.fandom.com/wiki/World_of_Warcraft_API
//https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/328263-wow-1-12-1-5875-info-dump-thread-5.html
//https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/305473-sample-code-endscene-hook-asm-blackmagic.html

//TO CALL FUNCTIONS
//https://www.unknowncheats.me/forum/c-and-c-/120848-asm-writeprocessmemory.html
//https://stackoverflow.com/questions/7397660/calling-asm-function-in-c
//https://stackoverflow.com/questions/3911578/how-to-call-c-functions-in-my-assembly-code
//https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/609516-1-12-1-c-calling-dostring-main-thread-using-endscene-hook.html

//ADDRESSES LIST
//CallAutoLoot 0x4C1FA0
//Endscene 0x5A17B6(009A17B6)

//IntPtr FrameScript_Execute = new IntPtr(0x00704CD0);(B04CD0)
//FrameScript::Register: 0x704120
//FrameScript::GetContext: 0x7040D0

#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <Tlhelp32.h>
using namespace std;
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Advapi32.lib")


void EnableDebugPrivilege()
{
	// Open the local process's privilege token
	void* Token = nullptr;
	TOKEN_PRIVILEGES Privileges;
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, reinterpret_cast<void**>(&Token)) == 0)
	{
		throw exception("Unable to open the local process token.");
	}
	
	// Set the value of SeDebugPrivilege to enabled
	Privileges.PrivilegeCount = 1;
	LookupPrivilegeValue(nullptr, "SeDebugPrivilege", &Privileges.Privileges[0].Luid);
	Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(AdjustTokenPrivileges(Token, 0, &Privileges, sizeof(Privileges), nullptr, nullptr) == 0)
	{
		CloseHandle(Token);
		throw exception("Unable to enable the debug privilege.");
	}
	CloseHandle(Token);
}

DWORD GetPID(char* procName)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	  
	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if( hProcessSnap == INVALID_HANDLE_VALUE )	
		return 0;
	
	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	if(!Process32First(hProcessSnap, &pe32))
		return 0;
	
	do
	{
		if(strcmp(pe32.szExeFile, procName) == 0)
		{
			DWORD pid = pe32.th32ProcessID;
			CloseHandle(hProcessSnap);
			return pid;
		}
	} while(Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return 0;
}


DWORD GetBaseAddress(DWORD pid)
{
	HANDLE phandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 module; 
	module.dwSize = sizeof(MODULEENTRY32);
	Module32First(phandle, &module);
	CloseHandle(phandle);
	return reinterpret_cast<unsigned int>(module.modBaseAddr);
}


typedef void (__fastcall * function_t)(int buffer, int key);
function_t pfTheFunc = (function_t)0x4C1FA0;
//IntPtr FrameScript_Execute = new IntPtr(0x704CD0);
//CallAutoLoot 0x4C1FA0
//__declspec(naked) void stub_start()
//void stub_end()

void __declspec(naked) __start()
{
	__asm
	{
		//CALL pfTheFunc//WILL CRASH
		
		//FRAMESCRIPT EXECUTE
		//MOV EAX, 0x00704CD0
		//CALL EAX

		//AUTOLOOT CRASHES SOMETIMES
		MOV EAX, 0x4C1FA0
		CALL EAX
		RET
	}
}
void __end() {}


void InjectModule()
{
	try
	{
		unsigned int ProcessID = GetPID("WoW.exe");
		printf("PID: %d\n", ProcessID);
		// Get a handle to the process
		void* ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);
		if(ProcessHandle == nullptr)
		{
			throw exception("Module injection failed: Unable to open a handle to the target process.");
		}
		
		//void* AllocatedMemory = VirtualAllocEx(ProcessHandle, nullptr, sizeof(int), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);//sizeof(int)
		//void* AllocatedMemory = VirtualAllocEx(ProcessHandle, nullptr, sizeof(int), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);//sizeof(int)
		LPVOID AllocatedMemory = VirtualAllocEx(ProcessHandle, nullptr, (DWORD)__end - (DWORD)__start, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		//LPVOID pLibRemote = VirtualAllocEx(ProcessHandle, NULL, sizeof(int), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );//cbCodeSize
		
		/*
		if(AllocatedMemory == nullptr)
		{
			CloseHandle(ProcessHandle);
			throw exception("Module injection failed: Unable to allocate memory in the target process.");
		}
		*/
		
		//WriteProcessMemory(ProcessHandle, AllocatedMemory, &parameter, sizeof(int), nullptr);
		//WriteProcessMemory(hProc, address, stub_start, stub_end-stub_start, &nBytes);
		WriteProcessMemory(ProcessHandle, AllocatedMemory, &__start, (DWORD)__end - (DWORD)__start, nullptr);//(DWORD)__end - (DWORD)__start
		//WriteProcessMemory(ProcessHandle, AllocatedMemory, &stub_start, sizeof(int), nullptr);
		
		//DWORD base = GetBaseAddress(ProcessID);
		//DWORD functionptr = base + 0x10000;               //Function offset may diff for diff compiler
		
		//CreateRemoteThread(ProcessHandle, nullptr, 0, (LPTHREAD_START_ROUTINE)0x400000, AllocatedMemory, 0, nullptr);
		//CreateRemoteThread(ProcessHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(functionptr), AllocatedMemory, 0, nullptr);
		CreateRemoteThread(ProcessHandle, nullptr, 0, (LPTHREAD_START_ROUTINE)AllocatedMemory, __start, 0, nullptr);
		//CreateRemoteThread(ProcessHandle, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, AllocatedMemory, 0, nullptr);//DLL?
		//CreateRemoteThread(ProcessHandle, nullptr, 0, (LPTHREAD_START_ROUTINE) pLibRemote, AllocatedMemory, 0, nullptr);

		/*
		if(ThreadHandle == nullptr)
		{
			CloseHandle(ProcessHandle);
			throw exception("Module injection failed: Unable to create thread in the target process.");
		}	
		PostQuitMessage(0);
		*/

		CloseHandle(ProcessHandle);
		VirtualFreeEx(ProcessHandle, (LPVOID) AllocatedMemory, 0, MEM_RELEASE);
	}

	catch(exception Exception)
	{
		puts(Exception.what());
	}

	return;
}


int main()
{
	EnableDebugPrivilege();

	/*
	HWND hWnd;
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	
	ZeroMemory(&sInfo, sizeof(sInfo));
	sInfo.cb = sizeof(sInfo);
	ZeroMemory(&pInfo, sizeof(pInfo));
	CreateProcess("F:\\Program Files (x86)\\World of Warcraft 1.12\\WoW.exe", "open", NULL, NULL, false, 0, NULL, NULL, &sInfo, &pInfo);
	*/


	InjectModule();

	printf("CLOSING!\n");
	_getch();

	return 0;
}