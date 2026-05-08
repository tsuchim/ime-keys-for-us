#pragma once

#include <windows.h>

constexpr UINT WM_APP_SPECULATIVE_IME_SET = WM_APP + 1;
constexpr UINT WM_APP_SPECULATIVE_IME_RESTORE = WM_APP + 2;
constexpr UINT WM_APP_SPECULATIVE_IME_COMMIT = WM_APP + 3;
constexpr UINT WM_APP_KEYBOARD_PENDING_CHANGED = WM_APP + 4;

constexpr WPARAM MakeImeGestureParam(DWORD gesture_id, bool open) {
  return (static_cast<WPARAM>(gesture_id) << 1) | (open ? 1u : 0u);
}

constexpr DWORD ImeGestureIdFromParam(WPARAM param) {
  return static_cast<DWORD>(param >> 1);
}

constexpr bool ImeOpenFromParam(WPARAM param) {
  return (param & 1u) != 0;
}
