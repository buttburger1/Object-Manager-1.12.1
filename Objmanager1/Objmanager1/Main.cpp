//WORK IN PROGRESS BY BUTTBURGER//

//REFERENCE
//https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/455847-c-unitname.html
//https://stackoverflow.com/questions/3019153/how-do-i-use-an-enum-value-in-a-switch-statement-in-c/3019195#3019195
//https://github.com/Zz9uk3/ZzukBot_V3/blob/master/ZzukBot_WPF/Objects/WoWUnit.cs#L199-L217
//https://github.com/Zz9uk3/CmdObjectManager/blob/master/CmdObjectManager/Mem/Manager.cs
//https://stackoverflow.com/questions/27203348/how-to-increment-decrement-hexadecimal-value-in-c
//https://www.cplusplus.com/reference/cstdio/printf/


#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <conio.h>


#define PI 3.141592653589793


DWORD pid;
HWND hwnd;
HANDLE phandle;

int camfov;

unsigned int isingame, currentobjtype, autoloot1 = 1, autoloot_off = 0, autoloot_on = 1,
	myhp, mymana,
	targethp, targetmana;

unsigned long long baseaddr, firstobj, nextobj, localguid, targetguid, currentguid, test;

float mycamxpos, mycamypos, mycamzpos, mycamxrot, mycamzrot,
	myx, myy, myz, myfacing, myspeed, myspeedrunmodifier,
	targetx, targety, targetz, targetfacing, targetspeed, targetspeedrunmodifier;

unsigned int testname;

float speedup = 14;


//NPC
char mycrapchar[50];
//GAMEOBJ
char mycrapchar1[50];


float ftest1, ftest2, ftest3;

//NPC
unsigned int mytest1, mytest2, mytest3;

//GAMEOBJ
unsigned int mystuff1, mystuff2, mystuff3;


int LoadPrivilege()
{
	HANDLE hToken;
	LUID Value;
	TOKEN_PRIVILEGES tp;
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken))return(GetLastError());
	if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Value))return(GetLastError());
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = Value;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(!AdjustTokenPrivileges(hToken, FALSE,&tp, sizeof(tp), NULL, NULL))return(GetLastError());
	CloseHandle(hToken);
	return 1;
}

enum Choice
{
	NONE = 0,
	ITEM = 1, 
	CONTAINER = 2, 
	UNIT = 3,
	PLAYER = 4,
	GAMEOBJECT = 5,
	DYNAMICOBJECT = 6,
	CORPSE = 7
};

int i = -1;

void objmanager()
{
	//////////////////////INACCURATE MUST CHANGE////////////////////////////////////
	ReadProcessMemory(phandle, (void*)0x00CE9704, &mycamxpos, sizeof(mycamxpos), 0);
	ReadProcessMemory(phandle, (void*)0x00CE9708, &mycamypos, sizeof(mycamypos), 0);
	ReadProcessMemory(phandle, (void*)0x00CE970C, &mycamzpos, sizeof(mycamzpos), 0);

	ReadProcessMemory(phandle, (void*)0x00CE9B8C, &mycamxrot, sizeof(mycamxrot), 0);
	ReadProcessMemory(phandle, (void*)0x00CE9B90, &mycamzrot, sizeof(mycamzrot), 0);
	////////////////////////////////////////////////////////////////////////////////
	ReadProcessMemory(phandle, (void*)0xB4B424, &isingame, sizeof(isingame), 0);
	ReadProcessMemory(phandle, (void*)(0xB41414), &baseaddr, sizeof(baseaddr), 0);
	ReadProcessMemory(phandle, (void*)(baseaddr + 0xAC), &firstobj, sizeof(firstobj), 0);
	ReadProcessMemory(phandle, (void*)(baseaddr + 0xC0), &localguid, sizeof(localguid), 0);
	//printf("LOCALGUID: %llu\n", localguid);
	ReadProcessMemory(phandle, (void*)(0xB4E2D8), &targetguid, sizeof(targetguid), 0);
	//if(targetguid == localguid)printf("TARGETGUID: %llu\n", targetguid);
	nextobj = firstobj;
	while(firstobj != 0 && (firstobj & 1) == 0)
	{
		ReadProcessMemory(phandle, (void*)(firstobj + 0x14), &currentobjtype, sizeof(currentobjtype), 0);
		//printf("currentobjtypenumber: %u\n", currentobjtype);
		ReadProcessMemory(phandle, (void*)(firstobj + 0x30), &currentguid, sizeof(currentguid), 0);
		switch(currentobjtype)//cin >> i;
		{
		case ITEM:
			//printf("ITEM\n");
			break;
		case CONTAINER:
			//printf("CONTAINER\n");
		case UNIT:
			printf("UNIT: 0x%.8X ", firstobj);
			ReadProcessMemory(phandle, (void*)(firstobj + 0xB30), &mytest1, sizeof(mytest1), 0);
			//printf("AAA: %u, ", mytest1);
			ReadProcessMemory(phandle, (void*)(mytest1 + 0x0), &mytest2, sizeof(mytest2), 0);
			//printf("BBB: %u, ", mytest2);
			ReadProcessMemory(phandle, (void*)(mytest2), &mycrapchar, sizeof(mycrapchar), 0);
			printf("CCC: %s\n", mycrapchar);
			break;
		case PLAYER:

			printf("PLAYER: 0x%.8X\n", firstobj);
			if(currentobjtype == 4)
			{
				char targetNameToChar[50] = "";
				int targetname = 0;
				int namebase = 0xC0E230;
				unsigned long long tempguid = 0;
				while(true)
				{
					ReadProcessMemory(phandle, (void*)(namebase), &targetname, sizeof(targetname), 0);
					ReadProcessMemory(phandle, (void*)(targetname + 0xC), &tempguid, sizeof(tempguid), 0);//targetname + 0xC;
					if(tempguid == 0)
					{
						printf("EMPTY!\n");
					}
					if(tempguid != currentguid)
					{
						ReadProcessMemory(phandle, (void*)(targetname), &namebase, sizeof(namebase), 0);
						targetname = namebase;
						//printf("Loop: %d!\n", namebase);
					}
					else
					{
						break;
					}
				}
				ReadProcessMemory(phandle, (void*)(targetname + 0x14), &targetNameToChar, sizeof(targetNameToChar), 0);
				printf("PLAYERNAME: %s\n", targetNameToChar);
			}

			break;
		case GAMEOBJECT:
			printf("GAMEOBJECT: 0x%.8X ", firstobj);
			
			if (baseaddr == 0 || currentguid == 0)
			{
				printf("NOTHING!\n");
			}
			else
			{
				//return BmWrapper.memory.ReadASCIIString((BmWrapper.memory.ReadUInt((BmWrapper.memory.ReadUInt(baseAdd + 0x214) + 0x8))), 40);
				ReadProcessMemory(phandle, (void*)(firstobj + 0x214), &mystuff1, sizeof(mystuff1), 0);
				//printf("AAA: %u, ", mystuff1);
				ReadProcessMemory(phandle, (void*)(mystuff1 + 0x8), &mystuff2, sizeof(mystuff2), 0);
				//printf("BBB: %u, ", mystuff2);
				ReadProcessMemory(phandle, (void*)(mystuff2), &mycrapchar1, sizeof(mycrapchar1), 0);
				printf("CCC: %s\n", mycrapchar1);
			}
			break;
		case DYNAMICOBJECT:
			printf("DYNAMICOBJECT: 0x%.8X\n", firstobj);
			break;
		case CORPSE:
			printf("CORPSE: 0x%.8X\n", firstobj);
			break;
		default:
			//printf("Invalid choice!\n");
			break;
		}


		if(currentguid == localguid)
		{
			printf("MY GUID: %llu\n", currentguid);
			ReadProcessMemory(phandle, (void*)(firstobj + 0x9B8), &myx, sizeof(myx), 0);
			ReadProcessMemory(phandle, (void*)(firstobj + 0x9BC), &myy, sizeof(myy), 0);
			ReadProcessMemory(phandle, (void*)(firstobj + 0x9C0), &myz, sizeof(myz), 0);
			printf("MY LOCATION: X: %f, Y: %f, Z: %f\n", myx, myy, myz);
			ReadProcessMemory(phandle, (void*)(firstobj + 0x1DC8), &myhp, sizeof(myhp), 0);
			ReadProcessMemory(phandle, (void*)(firstobj + 0x1DCC), &mymana, sizeof(mymana), 0);

			ReadProcessMemory(phandle, (void*)(firstobj + 0xA2C), &myspeed, sizeof(myspeed), 0);
			//WriteProcessMemory(phandle, (LPVOID)(firstobj + 0xA34), &speedup, sizeof(speedup), 0);
			ReadProcessMemory(phandle, (void*)(firstobj + 0xA34), &myspeedrunmodifier, sizeof(myspeedrunmodifier), 0);
			//printf("RUNSPEED: %f RUNSPEEDMODIFIER: %f\n", myspeed, myspeedrunmodifier);

			//WriteProcessMemory(phandle, (LPVOID)(0xC4DA9C), &autoloot_on, sizeof(autoloot_on), 0);
			//WriteProcessMemory(phandle, (LPVOID)(0xC4DA9C), &autoloot_off, sizeof(autoloot_off), 0);
			WriteProcessMemory(phandle, (LPVOID)(firstobj + 0x1D30), &autoloot1, sizeof(autoloot1), 0);
		}

		if(currentguid == targetguid)
		{
			ReadProcessMemory(phandle, (void*)(firstobj + 0x9B8), &targetx, sizeof(targetx), 0);
			ReadProcessMemory(phandle, (void*)(firstobj + 0x9BC), &targety, sizeof(targety), 0);
			ReadProcessMemory(phandle, (void*)(firstobj + 0x9C0), &targetz, sizeof(targetz), 0);
			//printf("MY LOCATION: X: %f, Y: %f, Z: %f\n", myx, myy, myz);

			ReadProcessMemory(phandle, (void*)(firstobj + 0x1DC8), &targethp, sizeof(targethp), 0);
			ReadProcessMemory(phandle, (void*)(firstobj + 0x1DCC), &targetmana, sizeof(targetmana), 0);
			//if(GetAsyncKeyState(VK_INSERT))
			//{
				//targetz -= 2;
				//WriteProcessMemory(phandle, (LPVOID)(firstobj + 0x9C0), &targetz, sizeof(targetz), 0);
			//}
		}

		ReadProcessMemory(phandle, (void*)(firstobj + 0x3C), &nextobj, sizeof(nextobj), 0);
		if(nextobj == firstobj)
		{
			//printf("BREAK!\n");
			break;
		}
		else
		{
			firstobj = nextobj;
		}
	}
}

void convertintchar(char*ch, int i)
{
	sprintf(ch, "%d", i);
}
void convertfloatchar(char*ch, float fl)
{
	sprintf(ch, "%f", fl);
}

int main()
{
	//REMEMBER TO SET UAC EXECUTION LEVEL TO ADMIN
	printf("Loading process access grabbing handle!\n");
	LoadPrivilege();
	hwnd = FindWindow(NULL,"World of Warcraft");
	GetWindowThreadProcessId(hwnd, &pid);
	phandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

	objmanager();

	_getch();
	return 0;
}