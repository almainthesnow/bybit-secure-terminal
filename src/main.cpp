#include <iostream>
#include <windows.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <json.hpp> // for JSON parsing

namespace fs = std::filesystem;
using json = nlohmann::json;

void printBanner() {
    std::cout << R"(
    ╔══════════════════════════════════════════╗
    ║  Bybit Secure Terminal v1.0              ║
    ║  OpenClaw Scanner & Protection            ║
    ╚══════════════════════════════════════════╝
    )" << std::endl;
}

bool isAdmin() {
    BOOL isElevated = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD size = sizeof(elevation);
        if (GetTokenInformation(hToken, TokenElevation, &elevation, size, &size))
            isElevated = elevation.TokenIsElevated;
        CloseHandle(hToken);
    }
    return isElevated;
}

void scanForOpenClaw() {
    std::cout << "\n[*] Scanning for OpenClaw malware..." << std::endl;
    
    // Known malicious process names from OpenClaw campaign
    std::vector<std::string> badProcesses = {
        "openclaw.exe", "bybit-plugin.exe", "polymarket-skill.exe",
        "clawhub.exe", "moltbot.exe", "bybit-ai.exe"
    };
    
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W proc;
        proc.dwSize = sizeof(proc);
        if (Process32FirstW(snap, &proc)) {
            do {
                std::wstring wname(proc.szExeFile);
                std::string name(wname.begin(), wname.end());
                for (const auto& bad : badProcesses) {
                    if (name.find(bad) != std::string::npos) {
                        std::cout << "[!] DETECTED: " << name << " (PID: " << proc.th32ProcessID << ")" << std::endl;
                    }
                }
            } while (Process32NextW(snap, &proc));
        }
        CloseHandle(snap);
    }
}

void scanBrowserExtensions() {
    std::cout << "\n[*] Checking browser extensions..." << std::endl;
    
    // Common extension paths
    std::vector<std::string> paths = {
        getenv("LOCALAPPDATA") + std::string("\\Google\\Chrome\\User Data\\Default\\Extensions"),
        getenv("LOCALAPPDATA") + std::string("\\Microsoft\\Edge\\User Data\\Default\\Extensions"),
        getenv("APPDATA") + std::string("\\Mozilla\\Firefox\\Profiles")
    };
    
    for (const auto& path : paths) {
        if (fs::exists(path)) {
            std::cout << "[*] Scanning: " << path << std::endl;
            // In real tool, would parse extension manifests
        }
    }
}

int main() {
    printBanner();

    if (!isAdmin()) {
        std::cout << "[!] Please run as Administrator for full scan.\n";
    }

    scanForOpenClaw();
    scanBrowserExtensions();

    std::cout << "\n[✓] Scan complete. No OpenClaw threats detected (simulated)." << std::endl;
    std::cout << "Launching secure terminal..." << std::endl;
    
    // Simulate terminal launch
    Sleep(2000);
    std::cout << "Secure terminal ready. Please log in with your API keys.\n";
    
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}