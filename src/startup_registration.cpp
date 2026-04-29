#include "startup_registration.h"

#include <windows.h>

#include <cwchar>
#include <iterator>

namespace {
constexpr wchar_t kRunKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr wchar_t kRunValueName[] = L"ImeKeysForUS";

bool GetCurrentExecutablePath(wchar_t* path, DWORD path_count) {
  DWORD length = GetModuleFileNameW(nullptr, path, path_count);
  return length > 0 && length < path_count;
}

bool BuildStartupCommand(wchar_t* command, DWORD command_count) {
  wchar_t exe_path[MAX_PATH]{};
  if (!GetCurrentExecutablePath(exe_path, MAX_PATH)) {
    return false;
  }

  int written =
      swprintf_s(command, command_count, L"\"%s\" --startup", exe_path);
  return written > 0;
}
}  // namespace

bool IsStartupEnabled() {
  HKEY key = nullptr;
  LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, kRunKey, 0, KEY_READ, &key);
  if (result != ERROR_SUCCESS) {
    return false;
  }

  wchar_t value[2 * MAX_PATH]{};
  DWORD value_size = sizeof(value);
  DWORD type = 0;
  result = RegGetValueW(key, nullptr, kRunValueName, RRF_RT_REG_SZ, &type,
                        value, &value_size);
  RegCloseKey(key);
  if (result != ERROR_SUCCESS) {
    return false;
  }

  wchar_t expected[2 * MAX_PATH]{};
  if (!BuildStartupCommand(expected, static_cast<DWORD>(std::size(expected)))) {
    return false;
  }
  return wcscmp(value, expected) == 0;
}

bool EnableStartup() {
  wchar_t command[2 * MAX_PATH]{};
  if (!BuildStartupCommand(command, static_cast<DWORD>(std::size(command)))) {
    return false;
  }

  HKEY key = nullptr;
  DWORD disposition = 0;
  LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, kRunKey, 0, nullptr, 0,
                                KEY_SET_VALUE, nullptr, &key, &disposition);
  if (result != ERROR_SUCCESS) {
    return false;
  }

  DWORD bytes = static_cast<DWORD>((wcslen(command) + 1) * sizeof(wchar_t));
  result = RegSetValueExW(key, kRunValueName, 0, REG_SZ,
                          reinterpret_cast<const BYTE*>(command), bytes);
  RegCloseKey(key);
  return result == ERROR_SUCCESS;
}

bool DisableStartup() {
  HKEY key = nullptr;
  LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, kRunKey, 0, KEY_SET_VALUE,
                              &key);
  if (result != ERROR_SUCCESS) {
    return result == ERROR_FILE_NOT_FOUND;
  }

  result = RegDeleteValueW(key, kRunValueName);
  RegCloseKey(key);
  return result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND;
}
