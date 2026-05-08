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
  target.hwnd = GetForegroundWindow();
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
  return SetOpenStatus(target, open, nullptr);
}

bool ImeController::SetOpenStatus(const Target& target, bool open,
                                  const bool* known_open) const {
  if (!IsSameTarget(target)) {
    return false;
  }

  bool before = false;
  bool has_before = false;
  if (known_open != nullptr) {
    before = *known_open;
    has_before = true;
  } else {
    has_before = GetOpenStatus(target, &before);
  }
  if (has_before && before == open) {
    return true;
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
