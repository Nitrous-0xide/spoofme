// "Nitroud is a skid" - Nitrous 2026
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <cfgmgr32.h>
#include <iostream>
#include <string>
#include <random>
#include <thread>
#include <chrono>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "cfgmgr32.lib")

void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void PrintLogo() {
    SetColor(13);
    std::cout << R"(
 ███████╗██████╗  ██████╗  ██████╗ ███████╗███╗   ███╗███████╗
 ██╔════╝██╔══██╗██╔═══██╗██╔═══██╗██╔════╝████╗ ████║██╔════╝
 ███████╗██████╔╝██║   ██║██║   ██║█████╗  ██╔████╔██║█████╗  
 ╚════██║██╔═══╝ ██║   ██║██║   ██║██╔══╝  ██║╚██╔╝██║██╔══╝  
 ███████║██║     ╚██████╔╝╚██████╔╝██║     ██║ ╚═╝ ██║███████╗
 ╚══════╝╚═╝      ╚═════╝  ╚═════╝ ╚═╝     ╚═╝     ╚═╝╚══════╝
)" << std::endl;
    SetColor(8);
    std::cout << "  [Mac spoofer // Stable-ish Build 2026]" << std::endl;
    SetColor(7);
}

void Status(std::string msg, int type = 0) {
    std::cout << "\n ";
    if (type == 0) { SetColor(13); std::cout << "[WAIT]"; }
    else if (type == 1) { SetColor(10); std::cout << "[DONE]"; }
    else { SetColor(12); std::cout << "[FAIL]"; }
    SetColor(7);
    std::cout << " " << msg;
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
}

void BootSequence() {
    SetColor(8);
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, 
                                  DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    std::cout << " [PRIVILEGE_CHECK] Administrator: " << (isAdmin ? "YES" : "NO") << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << " [SYSTEM_CHECK] OS Build: ";
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    #pragma warning(suppress: 4996)
    GetVersionEx((LPOSVERSIONINFO)&osvi);
    std::cout << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << "." << osvi.dwBuildNumber << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    SYSTEM_INFO sysInfo;
    GetNativeSystemInfo(&sysInfo);
    std::cout << " [ARCHITECTURE] " << (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x64" : "x86") << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (!isAdmin) {
        SetColor(12);
        std::cout << " [WARNING] Administrative privileges required for full functionality!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    SetColor(7);
}

std::string RandomHex(int len) {
    const char* hex = "0123456789ABCDEF";
    std::string res;
    std::random_device rd;
    for (int i = 0; i < len; ++i) res += hex[rd() % 16];
    return res;
}

void SpoofHwProfile() {
    std::string newGuid = "{" + RandomHex(8) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(12) + "}";
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "HwProfileGuid", 0, REG_SZ, (const BYTE*)newGuid.c_str(), static_cast<DWORD>(newGuid.length() + 1));
        RegCloseKey(hKey);
        Status("HwProfileGuid randomized.", 1);
    }
}

void SpoofNetwork() {
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_NET, nullptr, nullptr, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) return;

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); ++i) {
        char name[256];
        if (!SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, nullptr, (PBYTE)name, sizeof(name), nullptr))
            continue;

        if (strstr(name, "Miniport") || strstr(name, "Virtual")) continue;

        Status("Updating: " + std::string(name));

        HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DRV, KEY_WRITE);
        if (hKey != INVALID_HANDLE_VALUE) {
            std::string mac = RandomHex(12);
            mac[1] = "26AE"[rand() % 4];

            if (RegSetValueExA(hKey, "NetworkAddress", 0, REG_SZ, (const BYTE*)mac.c_str(), static_cast<DWORD>(mac.length() + 1)) == ERROR_SUCCESS) {
                SP_PROPCHANGE_PARAMS pcp = { sizeof(SP_CLASSINSTALL_HEADER), DIF_PROPERTYCHANGE, DICS_PROPCHANGE, DICS_FLAG_GLOBAL, 0 };
                SetupDiSetClassInstallParams(hDevInfo, &devInfoData, &pcp.ClassInstallHeader, sizeof(pcp));
                SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &devInfoData);
                std::cout << " -> [" << mac << "]";
            }
            RegCloseKey(hKey);
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    SetConsoleTitleA("SPOOFME :: IDENTITY SHIFTER");
    system("cls");

    PrintLogo();
    BootSequence();

    Status("Starting hardware re-identification...", 0);
    Status("Generating new Network ID (MAC)...", 0);
    SpoofNetwork();

    Status("Scrambling MachineGuid...", 0);
    std::string mGuid = RandomHex(8) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(12);
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_SET_VALUE | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "MachineGuid", 0, REG_SZ, (const BYTE*)mGuid.c_str(), static_cast<DWORD>(mGuid.length() + 1));
        RegCloseKey(hKey);
    }

    Status("Updating Registry HWID...", 0);
    SpoofHwProfile();

    std::cout << "\n\n";
    SetColor(13);
    std::cout << " [!] Operation Successful. Identity traces obfuscated." << std::endl;
    SetColor(7);
    std::cout << " Press ENTER to exit.";
    std::cin.get();

    return 0;
}
