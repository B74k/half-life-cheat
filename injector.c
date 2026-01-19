#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>

/*
 * Find process by name
 * Returns process ID or 0 if not found
 */
static DWORD find_process(const char* name) {
    HANDLE snapshot;
    PROCESSENTRY32 entry;
    DWORD pid = 0;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry)) {
        do {
            if (_stricmp(entry.szExeFile, name) == 0) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return pid;
}

/*
 * Inject DLL into process
 */
static int inject_dll(DWORD pid, const char* dll_path) {
    HANDLE process;
    HANDLE thread;
    void* remote_path;
    size_t path_len;
    LPVOID load_library;

    printf("[*] Opening process %lu...\n", pid);

    process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!process) {
        printf("[-] Failed to open process (error %lu)\n", GetLastError());
        return 0;
    }

    path_len = strlen(dll_path) + 1;

    printf("[*] Allocating memory in target process...\n");

    remote_path = VirtualAllocEx(process, NULL, path_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote_path) {
        printf("[-] Failed to allocate memory (error %lu)\n", GetLastError());
        CloseHandle(process);
        return 0;
    }

    printf("[*] Writing DLL path to target process...\n");

    if (!WriteProcessMemory(process, remote_path, dll_path, path_len, NULL)) {
        printf("[-] Failed to write memory (error %lu)\n", GetLastError());
        VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);
        CloseHandle(process);
        return 0;
    }

    printf("[*] Creating remote thread...\n");

    load_library = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (!load_library) {
        printf("[-] Failed to get LoadLibraryA address\n");
        VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);
        CloseHandle(process);
        return 0;
    }

    thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)load_library, remote_path, 0, NULL);
    if (!thread) {
        printf("[-] Failed to create remote thread (error %lu)\n", GetLastError());
        VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);
        CloseHandle(process);
        return 0;
    }

    printf("[*] Waiting for injection to complete...\n");

    WaitForSingleObject(thread, INFINITE);

    printf("[+] DLL injected successfully!\n");

    CloseHandle(thread);
    VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);
    CloseHandle(process);

    return 1;
}

int main(int argc, char* argv[]) {
    char dll_path[MAX_PATH];
    DWORD pid;

    printf("\n");
    printf("================================\n");
    printf("  Half-Life DLL Injector\n");
    printf("================================\n\n");

    /* Get DLL path */
    if (argc > 1) {
        strncpy_s(dll_path, sizeof(dll_path), argv[1], _TRUNCATE);
    } else {
        /* Default to hl_esp_bhop.dll in build/ folder */
        GetCurrentDirectoryA(sizeof(dll_path), dll_path);
        strcat_s(dll_path, sizeof(dll_path), "\\build\\hl_esp_bhop.dll");
    }

    /* Convert to full path */
    char full_path[MAX_PATH];
    if (!GetFullPathNameA(dll_path, sizeof(full_path), full_path, NULL)) {
        printf("[-] Invalid path: %s\n", dll_path);
        return 1;
    }

    printf("[*] DLL: %s\n", full_path);

    /* Check if DLL exists */
    if (GetFileAttributesA(full_path) == INVALID_FILE_ATTRIBUTES) {
        printf("[-] DLL file not found!\n");
        return 1;
    }

    /* Find hl.exe */
    printf("[*] Looking for hl.exe...\n");

    pid = find_process("hl.exe");
    if (!pid) {
        printf("[-] hl.exe not found! Make sure Half-Life is running.\n");
        return 1;
    }

    printf("[+] Found hl.exe (PID: %lu)\n", pid);

    /* Inject */
    if (!inject_dll(pid, full_path)) {
        printf("[-] Injection failed!\n");
        return 1;
    }

    printf("\n[+] Done! Check the game for the debug console.\n\n");

    return 0;
}
