#include "ime_controller.h"

#include <windows.h>
#include <imm.h>

#ifndef IMC_SETOPENSTATUS
#define IMC_SETOPENSTATUS 0x0006
#endif

bool ImeController::SetOpenStatus(bool open) const {
  HWND foreground = GetForegroundWindow();
  if (foreground == nullptr) {
    return false;
  }

  HWND ime_window = ImmGetDefaultIMEWnd(foreground);
  if (ime_window == nullptr) {
    return false;
  }

  LRESULT result = SendMessageW(ime_window, WM_IME_CONTROL, IMC_SETOPENSTATUS,
                                open ? 1 : 0);
  return result == 0;
}
