#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

using namespace std;

bool Inject(DWORD pId, char *dllName){
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, false, pId);
	if(h)
	{
		LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
			//Initialize memory allocation
			LPVOID dereercomp = VirtualAllocEx(h, NULL, strlen(dllName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			//Write DLL to memory
			WriteProcessMemory(h, dereercomp, dllName, strlen(dllName), NULL);
			HANDLE asdc = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
			//Wait for DLL to exit
			WaitForSingleObject(asdc, INFINITE);
			//Free memory
			VirtualFreeEx(h, dereercomp, strlen(dllName), MEM_RELEASE);
			CloseHandle(asdc);
			CloseHandle(h);
			return true;
	}
	return false;
}

int PID(const char *filename){
	
PROCESSENTRY32 pe32 = {0};

HANDLE hSnap;

int iDone;

bool bProcessFound;
	
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe32.dwSize = sizeof(PROCESSENTRY32); 
	Process32First(hSnap,&pe32);
	
	bProcessFound = false;
	iDone = 1;
	
	while(iDone)
	{
		iDone = Process32Next(hSnap,&pe32);
		if (strcmp(pe32.szExeFile,filename) == 0)
		{
			bProcessFound = true;
			iDone = 0;
		}
	}
	
	if(bProcessFound){
		//cout << "PID: " << pe32.th32ProcessID << endl;
		return pe32.th32ProcessID;
	}else{
		return 000000;
	}
}

int main(){

	while(true){
	
		string strDLL;
		string process;
		cout << "Enter executable name: ";
		cin >> process;
		int pid = PID( process.c_str() );
		TCHAR full_path[MAX_PATH];
		GetFullPathName("<PATH TO DLL>", MAX_PATH, full_path, NULL);
		Inject(pid, full_path);
	
	}
}
