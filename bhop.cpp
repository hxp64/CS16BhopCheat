#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "offsets.h"

DWORD GetProcessIdByName(const std::string& processName) {
    PROCESSENTRY32 pt;
    pt.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(hSnap, &pt)) {
        do {
            if (!processName.compare(pt.szExeFile)) {
                CloseHandle(hSnap);
                return pt.th32ProcessID;
            }
        } while (Process32Next(hSnap, &pt));
    }
    CloseHandle(hSnap);
    return 0;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const std::string& modName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!modName.compare(modEntry.szModule)) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

void RunLogic(const BuildConfig& cfg) {
    DWORD procId = 0;
    std::cout << "Waiting for " << cfg.exeName << "...\n";
    while (!procId) {
        procId = GetProcessIdByName(cfg.exeName);
        Sleep(1000);
    }

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, procId);
    if (!hProcess) return;

    std::cout << "Successfully attached to " << cfg.exeName << "\n";

    uintptr_t onGroundBase = 0, forceJumpBase = 0, inMenuBase = 0, isChattingBase = 0;

    while (!onGroundBase || !forceJumpBase || !inMenuBase || !isChattingBase) {
        onGroundBase = GetModuleBaseAddress(procId, cfg.onGround.moduleName);
        forceJumpBase = GetModuleBaseAddress(procId, cfg.forceJump.moduleName);
        inMenuBase = GetModuleBaseAddress(procId, cfg.inMenu.moduleName);
        isChattingBase = GetModuleBaseAddress(procId, cfg.isChatting.moduleName);
        Sleep(500);
    }

    uintptr_t addrOnGround = onGroundBase + cfg.onGround.offset;
    uintptr_t addrForceJump = forceJumpBase + cfg.forceJump.offset;
    uintptr_t addrInMenu = inMenuBase + cfg.inMenu.offset;
    uintptr_t addrIsChatting = isChattingBase + cfg.isChatting.offset;

    int lastState = 0;

    while (true) {
        DWORD exitCode = 0;
        if (GetExitCodeProcess(hProcess, &exitCode) && exitCode != STILL_ACTIVE) {
            std::cout << "WARNING: " << cfg.exeName << " HAS BEEN KILLED, returning to main...\n";
            break;
        }

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            int mActive = 0, cActive = 0;
            ReadProcessMemory(hProcess, (LPCVOID)addrInMenu, &mActive, sizeof(mActive), nullptr);
            ReadProcessMemory(hProcess, (LPCVOID)addrIsChatting, &cActive, sizeof(cActive), nullptr);

            if (mActive == 1) {
                if (lastState != 1) {
                    std::cout << "paused: MENU detected\n";
                    lastState = 1;
                }
                Sleep(5);
                continue;
            }

            if (cActive == 1) {
                if (lastState != 2) {
                    std::cout << "paused: CHAT detected\n";
                    lastState = 2;
                }
                Sleep(5);
                continue;
            }

            lastState = 0;

            int onGround = 0;
            ReadProcessMemory(hProcess, (LPCVOID)addrOnGround, &onGround, sizeof(onGround), nullptr);

            if (onGround == 0) {
                int forceJumpVal = 4;
                WriteProcessMemory(hProcess, (LPVOID)addrForceJump, &forceJumpVal, sizeof(forceJumpVal), nullptr);

                while (true) {
                    ReadProcessMemory(hProcess, (LPCVOID)addrOnGround, &onGround, sizeof(onGround), nullptr);
                    if (onGround != 0 || !(GetAsyncKeyState(VK_SPACE) & 0x8000)) break;
                    Sleep(1);
                }
                forceJumpVal = 5;
                WriteProcessMemory(hProcess, (LPVOID)addrForceJump, &forceJumpVal, sizeof(forceJumpVal), nullptr);
                std::cout << "Jump executed!\n";
            }
        }
        Sleep(5);
    }
    CloseHandle(hProcess);
    Sleep(1000);
}