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

ImeController::Target MakeTarget(HWND hwnd) {
  ImeController::Target target{};
  if (hwnd == nullptr || !IsWindow(hwnd)) {
    return target;
  }

  target.hwnd = hwnd;
  target.thread_id = GetWindowThreadProcessId(target.hwnd, &target.process_id);
  if (target.thread_id == 0) {
    target = {};
  }
  return target;
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

ImeController::Target ImeController::CaptureTarget(HWND foreground) {
  Target foreground_target = MakeTarget(foreground);
  if (foreground_target.hwnd == nullptr) {
    return foreground_target;
  }

  GUITHREADINFO gui_thread_info{};
  gui_thread_info.cbSize = sizeof(gui_thread_info);
  if (GetGUIThreadInfo(foreground_target.thread_id, &gui_thread_info) &&
      gui_thread_info.hwndFocus != nullptr &&
      (gui_thread_info.hwndFocus == foreground ||
       IsChild(foreground, gui_thread_info.hwndFocus))) {
    Target focus_target = MakeTarget(gui_thread_info.hwndFocus);
    if (focus_target.hwnd != nullptr) {
      return focus_target;
    }
  }

  return foreground_target;
}

ImeController::Target ImeController::CaptureForegroundTarget() {
  return CaptureTarget(GetForegroundWindow());
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
  if (ime_window != nullptr) {
    DWORD_PTR result = 0;
    if (SendImeControl(ime_window, IMC_SETOPENSTATUS, open ? 1 : 0, &result) &&
        result == 0) {
      bool after = false;
      if (GetOpenStatus(target, &after) && after == open) {
        return true;
      }
    }
  }

  return false;
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
