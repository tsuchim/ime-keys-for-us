#pragma once

#include <windows.h>

constexpr DWORD DEFAULT_ALT_DOUBLE_TAP_MS = 100;

struct AppSettings {
  DWORD alt_double_tap_ms = DEFAULT_ALT_DOUBLE_TAP_MS;
};

AppSettings LoadSettings();
