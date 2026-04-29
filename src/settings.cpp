#include "settings.h"

#include <shlobj.h>

#include <cwchar>

namespace {
constexpr wchar_t kSettingsDirectory[] = L"ImeKeysForUS";
constexpr wchar_t kSettingsFile[] = L"settings.ini";

DWORD ClampDoubleTapMs(int value) {
  if (value < static_cast<int>(MIN_ALT_DOUBLE_TAP_MS)) {
    return MIN_ALT_DOUBLE_TAP_MS;
  }
  if (value > static_cast<int>(MAX_ALT_DOUBLE_TAP_MS)) {
    return MAX_ALT_DOUBLE_TAP_MS;
  }
  return static_cast<DWORD>(value);
}

bool GetSettingsPath(wchar_t* path, DWORD path_count) {
  PWSTR appdata = nullptr;
  HRESULT result =
      SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr,
                           &appdata);
  if (FAILED(result) || appdata == nullptr) {
    return false;
  }

  int written = swprintf_s(path, path_count, L"%s\\%s\\%s", appdata,
                           kSettingsDirectory, kSettingsFile);
  CoTaskMemFree(appdata);
  return written > 0;
}
}  // namespace

AppSettings LoadSettings() {
  AppSettings settings{};

  wchar_t path[MAX_PATH]{};
  if (!GetSettingsPath(path, MAX_PATH)) {
    return settings;
  }

  int value = GetPrivateProfileIntW(L"Keyboard", L"DoubleTapMs",
                                    DEFAULT_ALT_DOUBLE_TAP_MS, path);
  settings.alt_double_tap_ms = ClampDoubleTapMs(value);
  return settings;
}
