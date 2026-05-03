#include "settings.h"

#include <shlobj.h>

#include <cwctype>
#include <iterator>
#include <string>

namespace {
constexpr wchar_t kSettingsDirectory[] = L"ImeKeysForUS";
constexpr wchar_t kSettingsFile[] = L"settings.ini";
constexpr DWORD kMaxSafeDoubleTapMs = 0x7fffffff;

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

bool TryParseDoubleTapMs(const std::wstring& text, DWORD* value) {
  if (text.empty()) {
    return false;
  }

  DWORD parsed = 0;
  for (wchar_t ch : text) {
    if (ch < L'0' || ch > L'9') {
      return false;
    }
    DWORD digit = static_cast<DWORD>(ch - L'0');
    if (parsed > (kMaxSafeDoubleTapMs - digit) / 10) {
      return false;
    }
    parsed = parsed * 10 + digit;
  }

  if (parsed == 0) {
    return false;
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
  DWORD value = 0;
  std::wstring trimmed = Trim(value_text);
  if (chars > 0 && TryParseDoubleTapMs(trimmed, &value)) {
    settings.alt_double_tap_ms = value;
  }
  return settings;
}
