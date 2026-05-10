#include <iostream>
#include <Windows.h>
#include "offsets.h"

bool IsAdmin() {
    BOOL isElevated = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &cbSize)) {
            isElevated = elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return isElevated;
}

int main(int argc, char* argv[]) {
    if (!IsAdmin()) {
        char szPath[MAX_PATH];
        if (GetModuleFileNameA(NULL, szPath, ARRAYSIZE(szPath))) {
            SHELLEXECUTEINFOA sei = { sizeof(sei) };
            sei.lpVerb = "runas";
            sei.lpFile = szPath;
            sei.hwnd = NULL;
            sei.nShow = SW_NORMAL;
            if (!ShellExecuteExA(&sei)) {
                return 1;
            }
            return 0;
        }
    }

    while (true) {
        std::cout << "\n--- Select Game Build ---\n";
        for (size_t i = 0; i < BUILDS.size(); ++i) {
            std::cout << "[" << i << "] " << BUILDS[i].name << "\n";
        }
        std::cout << "Choice: ";
        int choice;
        if (std::cin >> choice && choice >= 0 && choice < BUILDS.size()) {
            RunLogic(BUILDS[choice]);
        }
        else {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
    }
    return 0;
}