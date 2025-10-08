#pragma once
#include <windows.h>
#include <stdio.h>


// Thanks cr0w
typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define ok(strong, ...) (printf("\033[32m[+] " strong "\033[0m\n", ##__VA_ARGS__))
#define info(strong, ...) (printf("\033[36m[*] " strong "\033[0m\n", ##__VA_ARGS__))
#define error(strong, ...) (printf("\033[31m[-] " strong "\nError Code: %ld\n\033[0m", ##__VA_ARGS__, GetLastError()))
#define yolo()                              \
    printf("[*] Now Exiting. T-T See Ya."); \
    return EXIT_FAILURE;
