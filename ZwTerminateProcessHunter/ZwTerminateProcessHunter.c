#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

DWORD RvaToOffset(DWORD rva, PIMAGE_NT_HEADERS ntHeaders) {
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
	WORD numSections = ntHeaders->FileHeader.NumberOfSections;
	for (WORD i = 0; i < numSections; i++, section++) {
		DWORD secVA = section->VirtualAddress;
		// use virtual size (on-disk size may be smaller)
		DWORD secSize = max(section->Misc.VirtualSize, section->SizeOfRawData);
		if (rva >= secVA && rva < secVA + secSize) {
			return rva - secVA + section->PointerToRawData;
		}
	}
	return 0;
}

void ParseDriver(char* driverPath) {
	char* driverName = strrchr(driverPath, '\\');

#ifdef _DEBUG
	info("Targetting driver %s", driverName);
#endif
	
	char* targetFunction = "ZwTerminateProcess";

	FILE* driverFile = fopen(driverPath, "rb");
	if (!driverFile) {
		error("Failed to open driver file %s", driverPath);
		yolo();
	}
	// Read the driver file into memory
	fseek(driverFile, 0, SEEK_END);
	long fileSize = ftell(driverFile);
	rewind(driverFile);
	unsigned char* fileBuffer = (unsigned char*)malloc(fileSize);
	if (!fileBuffer) {
		error("Failed to allocate memory for driver file %s", driverPath);
		fclose(driverFile);
		yolo();
	}
	size_t bytesRead = fread(fileBuffer, 1, fileSize, driverFile);
	if (bytesRead != fileSize) {
		error("Failed to read driver file %s", driverPath);
		free(fileBuffer);
		fclose(driverFile);
		return;
	}
	fclose(driverFile);

	// Parse the PE headers
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
#ifdef _DEBUG
		error("Invalid DOS signature in driver file %s", driverPath);
#endif
		free(fileBuffer);
		return;
	}
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)(fileBuffer + dosHeader->e_lfanew);
	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
#ifdef _DEBUG
		error("Invalid NT signature in driver file %s", driverPath);
#endif
		free(fileBuffer);
		return;
	}
	PIMAGE_OPTIONAL_HEADER optionalHeader = &ntHeaders->OptionalHeader;
	if (optionalHeader->Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
#ifdef _DEBUG
		error("Unsupported PE format in driver file %s (only x64 supported)", driverPath);
#endif
		free(fileBuffer);
		return;
	}

	// Locate the import directory
	DWORD ImageBase = optionalHeader->ImageBase;
	DWORD importDirRVA = optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	DWORD importDirSize = optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
	if (importDirRVA == 0 || importDirSize == 0) {
		error("No import directory found in driver file %s", driverPath);
		free(fileBuffer);
		yolo();
	}
	
	DWORD Offset = RvaToOffset(importDirRVA, ntHeaders);
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)fileBuffer + Offset);
	// Iterate over the import descriptors to find the target function
	char* lib = "ntoskrnl.exe";
	if(importDescriptor->Name == 0) {
		error("No imports found in driver file %s", driverPath);
		free(fileBuffer);
		yolo();
	}
	while (importDescriptor->Name) {
		char* dllName = (char*)(fileBuffer + RvaToOffset(importDescriptor->Name,ntHeaders));
		if (_stricmp(dllName, lib) == 0) {
			// Found the target DLL, now search for the function
			DWORD* thunkRef = (DWORD*)(fileBuffer + RvaToOffset(importDescriptor->OriginalFirstThunk,ntHeaders));
			DWORD* funcRef = (DWORD*)(fileBuffer + RvaToOffset(importDescriptor->FirstThunk,ntHeaders));
			if (!thunkRef) {
				thunkRef = funcRef;
			}
			while (*thunkRef) {
				PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)(fileBuffer + RvaToOffset(*thunkRef,ntHeaders));
				if (strcmp((char*)importByName->Name, targetFunction) == 0) {
					// Found the target function
					ok("Found %s in %s", targetFunction, driverPath);
					free(fileBuffer);
					return;
				}
				(*thunkRef)++;
				(*funcRef)++;
			}
		}
		importDescriptor++;
	}

}

int NotDriver(const char* fileName) {
	const char* ext = strrchr(fileName, '.');
	if (!ext) {
		return 1; // No extension, not a driver
	} else if(strcmp(ext,".sys")==0) {
		return 0; // Is a driver
	}
	else {
		return 1; // Not a driver
	}
		
}

void ParseDirectory(const char* directoryPath) {
#ifdef _DEBUG
	info("Parsing directory: %s", directoryPath);
#endif
	WIN32_FIND_DATAA findFileData = {NULL};
	HANDLE hFind;
	char searchPath[MAX_PATH] = {NULL};
	snprintf(searchPath, MAX_PATH, "%s\\*.*", directoryPath);
	hFind = FindFirstFileA(&searchPath, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		error("FindFirstFile failed");
		yolo();
	}
	do {
		if (findFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findFileData.cFileName,".")==0 || strcmp(findFileData.cFileName,"..")==0) {
				continue;
			}
			char folderPath[MAX_PATH] = { NULL };
			snprintf(folderPath, MAX_PATH, "%s\\%s", directoryPath, findFileData.cFileName);
			ParseDirectory(folderPath);
		}
		else if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (NotDriver(findFileData.cFileName)) {
				continue;
			}
			char driverPath[MAX_PATH] = { NULL };
			snprintf(driverPath, MAX_PATH, "%s\\%s", directoryPath, findFileData.cFileName);
			ParseDriver(driverPath);
		}
	} while (FindNextFileA(hFind, &findFileData) != 0);
	FindClose(hFind);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
#ifndef _DEBUG
		error("Please provide a directory with drivers like so\n%s C:\\Users\\Admin\\Desktop\\DriverDirectory", argv[0]);
		yolo();
#else
		info("No directory provided, using debug path D:\\Drivers");
		ParseDirectory("D:\\BYOVD");
#endif
	}
	else {
		info("Recursively Targetting %s", argv[1]);
	}

	ParseDirectory(argv[1]);
}