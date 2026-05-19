#pragma once
#include <string>
#include <vector>

struct PointerData {
    std::string moduleName;
    uintptr_t offset;
};

struct BuildConfig {
    std::string name;
    std::string exeName;
    PointerData onGround;
    PointerData forceJump;
    PointerData inMenu;
    PointerData isChatting;
};

const std::vector<BuildConfig> BUILDS = {
    {"3266", "hl.exe", {"hl.exe", 0x1B70A54}, {"hl.exe", 0x6272A8}, {"hl.exe", 0xC706E8}, {"hl.exe", 0xFCE198}},
    {"4554", "hl.exe", {"hw.dll", 0x11FDAB4}, {"client.dll", 0x12D048}, {"hw.dll", 0x326800}, {"hw.dll", 0x684210}},
    {"8684", "hl.exe", {"hw.dll", 0x122E2D4}, {"client.dll", 0x131434}, {"hw.dll", 0x6C3AB0}, {"hw.dll", 0x64429C}},
    {"GSClient", "cs.exe", {"engine.dll", 0x122DF54}, {"client.dll", 0x131424}, {"engine.dll", 0x6C3730}, {"engine.dll", 0x643F1C}},
    {"NextClient", "cs.exe", {"hw.dll", 0x122E2D4}, {"client.dll", 0x131434}, {"hw.dll", 0x6C3AB0}, {"hw.dll", 0x64429C}},
    {"GoldClient", "hl.exe", {"hw.dll", 0x2F49620}, {"client.dll", 0x1A0EA8}, {"hw.dll", 0x32E6DE8}, {"hw.dll", 0x278FFFC}} // New since May 18, 2026
};

void RunLogic(const BuildConfig& cfg);
