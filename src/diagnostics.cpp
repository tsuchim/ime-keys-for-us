#include "diagnostics.h"

#include <shlobj.h>

#include <cwchar>
#include <string>

namespace {
constexpr wchar_t kDiagnosticsDirectory[] = L"ImeKeysForUS";
constexpr wchar_t kDiagnosticsFile[] = L"ime-keys-for-us.log";

bool GetDiagnosticsDirectory(wchar_t* path, DWORD path_count) {
  PWSTR local_appdata = nullptr;
  HRESULT result = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT,
                                        nullptr, &local_appdata);
  if (FAILED(result) || local_appdata == nullptr) {
    return false;
  }

  int written =
      swprintf_s(path, path_count, L"%s\\%s", local_appdata,
                 kDiagnosticsDirectory);
  CoTaskMemFree(local_appdata);
  return written > 0;
}

bool GetDiagnosticsPath(wchar_t* path, DWORD path_count) {
  wchar_t directory[MAX_PATH]{};
  if (!GetDiagnosticsDirectory(directory, MAX_PATH)) {
    return false;
  }

  CreateDirectoryW(directory, nullptr);
  int written =
      swprintf_s(path, path_count, L"%s\\%s", directory, kDiagnosticsFile);
  return written > 0;
}

std::wstring FormatError(DWORD error) {
  if (error == ERROR_SUCCESS) {
    return L"error=0";
  }

  wchar_t* buffer = nullptr;
  DWORD length = FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, error, 0, reinterpret_cast<wchar_t*>(&buffer), 0, nullptr);
  if (length == 0 || buffer == nullptr) {
    wchar_t fallback[64]{};
    swprintf_s(fallback, L"error=%lu", error);
    return fallback;
  }

  std::wstring message(buffer, length);
  LocalFree(buffer);
  while (!message.empty() &&
         (message.back() == L'\r' || message.back() == L'\n' ||
          message.back() == L' ')) {
    message.pop_back();
  }

  wchar_t prefix[64]{};
  swprintf_s(prefix, L"error=%lu: ", error);
  return std::wstring(prefix) + message;
}

std::wstring Timestamp() {
  SYSTEMTIME time{};
  GetLocalTime(&time);

  wchar_t text[64]{};
  swprintf_s(text, L"%04u-%02u-%02u %02u:%02u:%02u.%03u",
             time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
             time.wSecond, time.wMilliseconds);
  return text;
}

void AppendLogLine(const std::wstring& line) {
  wchar_t path[MAX_PATH]{};
  if (!GetDiagnosticsPath(path, MAX_PATH)) {
    return;
  }

  HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return;
  }

  std::wstring text = Timestamp() + L" " + line + L"\r\n";
  int bytes_needed =
      WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr,
                          nullptr);
  if (bytes_needed > 1) {
    std::string utf8(static_cast<size_t>(bytes_needed), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, utf8.data(),
                        bytes_needed, nullptr, nullptr);
    DWORD written = 0;
    WriteFile(file, utf8.data(), static_cast<DWORD>(bytes_needed - 1),
              &written, nullptr);
  }
  CloseHandle(file);
}
}  // namespace

void LogStartupEvent(const wchar_t* message) {
  AppendLogLine(message != nullptr ? message : L"");
}

void LogLastError(const wchar_t* stage, DWORD error) {
  std::wstring line = stage != nullptr ? stage : L"";
  line += L" ";
  line += FormatError(error);
  AppendLogLine(line);
}

void ShowFatalStartupError(const wchar_t* stage, DWORD error) {
  LogLastError(stage, error);

  std::wstring message = L"IME Keys for US failed to start.\r\n\r\n";
  message += stage != nullptr ? stage : L"Startup failed.";
  message += L"\r\n";
  message += FormatError(error);
  message +=
      L"\r\n\r\nLog: %LOCALAPPDATA%\\ImeKeysForUS\\ime-keys-for-us.log";

  MessageBoxW(nullptr, message.c_str(), L"IME Keys for US",
              MB_ICONERROR | MB_OK);
}

void ShowTrayIconStartupWarning(DWORD error) {
  LogLastError(L"Tray icon add failed.", error);

  std::wstring message =
      L"IME Keys for US started, but the tray icon could not be added.\r\n\r\n";
  message +=
      L"The app may still be running, but the tray menu will not be available.";
  message += L"\r\n";
  message += FormatError(error);
  message +=
      L"\r\n\r\nLog: %LOCALAPPDATA%\\ImeKeysForUS\\ime-keys-for-us.log";

  MessageBoxW(nullptr, message.c_str(), L"IME Keys for US",
              MB_ICONWARNING | MB_OK);
}
