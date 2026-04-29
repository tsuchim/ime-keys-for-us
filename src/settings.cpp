#include "settings.h"

#include <shlobj.h>

#include <cwctype>
#include <iterator>
#include <string>

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

std::wstring Trim(const wchar_t* value) {
  std::wstring text = value != nullptr ? value : L"";
  size_t first = 0;
  while (first < text.size() && iswspace(text[first])) {
    ++first;
  }

  size_t last = text.size();
  while (last > first && iswspace(text[last - 1])) {
    --last;
  }

  return text.substr(first, last - first);
}

bool TryParseStrictInt(const std::wstring& text, int* value) {
  if (text.empty()) {
    return false;
  }

  int parsed = 0;
  for (wchar_t ch : text) {
    if (ch < L'0' || ch > L'9') {
      return false;
    }
    parsed = parsed * 10 + (ch - L'0');
    if (parsed > 10000) {
      return false;
    }
  }

  *value = parsed;
  return true;
}
}  // namespace

AppSettings LoadSettings() {
  AppSettings settings{};

  wchar_t path[MAX_PATH]{};
  if (!GetSettingsPath(path, MAX_PATH)) {
    return settings;
  }

  wchar_t value_text[32]{};
  DWORD chars = GetPrivateProfileStringW(L"Keyboard", L"DoubleTapMs", L"",
                                         value_text,
                                         static_cast<DWORD>(std::size(value_text)),
                                         path);
  int value = 0;
  std::wstring trimmed = Trim(value_text);
  if (chars > 0 && TryParseStrictInt(trimmed, &value)) {
    settings.alt_double_tap_ms = ClampDoubleTapMs(value);
  }
  return settings;
}
