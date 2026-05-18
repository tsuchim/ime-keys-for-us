#include "ime_controller.h"

#include <windows.h>
#include <imm.h>

#ifndef IMC_GETOPENSTATUS
#define IMC_GETOPENSTATUS 0x0005
#endif

#ifndef IMC_SETOPENSTATUS
#define IMC_SETOPENSTATUS 0x0006
#endif

namespace {
constexpr UINT kImeMessageTimeoutMs = 100;

HWND ResolveFocusedHwnd(HWND foreground) {
  if (foreground == nullptr || !IsWindow(foreground)) {
    return nullptr;
  }

  DWORD foreground_process_id = 0;
  DWORD foreground_thread_id =
      GetWindowThreadProcessId(foreground, &foreground_process_id);
  if (foreground_thread_id == 0) {
    return foreground;
  }

  GUITHREADINFO info{};
  info.cbSize = sizeof(info);
  if (GetGUIThreadInfo(foreground_thread_id, &info) &&
      info.hwndFocus != nullptr && IsWindow(info.hwndFocus)) {
    DWORD focus_process_id = 0;
    DWORD focus_thread_id =
        GetWindowThreadProcessId(info.hwndFocus, &focus_process_id);
    if (focus_thread_id == foreground_thread_id &&
        focus_process_id == foreground_process_id) {
      return info.hwndFocus;
    }
  }

  return foreground;
}

bool SendImeControl(HWND ime_window, WPARAM command, LPARAM data,
                    DWORD_PTR* result) {
  if (result != nullptr) {
    *result = 0;
  }
  DWORD_PTR local_result = 0;
  LRESULT sent =
      SendMessageTimeoutW(ime_window, WM_IME_CONTROL, command, data,
                          SMTO_ABORTIFHUNG, kImeMessageTimeoutMs,
                          &local_result);
  if (sent == 0) {
    return false;
  }
  if (result != nullptr) {
    *result = local_result;
  }
  return true;
}

}  // namespace

ImeController::Target ImeController::CaptureForegroundTarget() {
  Target target{};
  HWND foreground = GetForegroundWindow();
  target.hwnd = ResolveFocusedHwnd(foreground);
  if (target.hwnd != nullptr) {
    target.thread_id = GetWindowThreadProcessId(target.hwnd, &target.process_id);
  }
  return target;
}

bool ImeController::SetOpenStatus(bool open) const {
  return SetOpenStatus(CaptureForegroundTarget(), open);
}

bool ImeController::GetOpenStatus(const Target& target, bool* open) const {
  if (open == nullptr || !IsSameTarget(target)) {
    return false;
  }

  HWND ime_window = ImmGetDefaultIMEWnd(target.hwnd);
  if (ime_window == nullptr) {
    return false;
  }

  DWORD_PTR result = 0;
  if (!SendImeControl(ime_window, IMC_GETOPENSTATUS, 0, &result)) {
    return false;
  }

  *open = result != 0;
  return true;
}

bool ImeController::SetOpenStatus(const Target& target, bool open) const {
  if (!IsSameTarget(target)) {
    return false;
  }

  HWND ime_window = ImmGetDefaultIMEWnd(target.hwnd);
  if (ime_window == nullptr) {
    return false;
  }

  DWORD_PTR result = 0;
  if (!SendImeControl(ime_window, IMC_SETOPENSTATUS, open ? 1 : 0, &result) ||
      result != 0) {
    return false;
  }

  bool after = false;
  return GetOpenStatus(target, &after) && after == open;
}

bool ImeController::IsSameTarget(const Target& target) const {
  if (target.hwnd == nullptr || !IsWindow(target.hwnd)) {
    return false;
  }

  DWORD process_id = 0;
  DWORD thread_id = GetWindowThreadProcessId(target.hwnd, &process_id);
  return thread_id != 0 && thread_id == target.thread_id &&
         process_id == target.process_id;
}
