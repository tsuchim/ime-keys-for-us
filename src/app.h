#pragma once

#include "keyboard_hook.h"
#include "ime_controller.h"
#include "settings.h"
#include "tray_icon.h"

#include <windows.h>

class App {
 public:
  explicit App(HINSTANCE instance);
  ~App();

  bool Initialize();
  int Run();

  static const wchar_t* WindowClassName();
  static UINT RefreshTrayMessageId();

 private:
  bool FailStartup(const wchar_t* stage, DWORD error);
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam,
                                     LPARAM lparam);
  LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam,
                        LPARAM lparam);
  void RefreshTrayIcon();
  void UpdateKeyboardTimer(HWND hwnd);
  void BeginSpeculativeImeSet(DWORD gesture_id, HWND target_hwnd, bool open);
  void RestoreSpeculativeIme(DWORD gesture_id);
  void CommitSpeculativeIme(DWORD gesture_id);

  struct SpeculativeImeState {
    DWORD gesture_id = 0;
    ImeController::Target target{};
    bool original_open = false;
    bool can_restore = false;
  };

  HINSTANCE instance_;
  HWND hwnd_ = nullptr;
  TrayIcon tray_icon_;
  KeyboardHook keyboard_hook_;
  ImeController ime_controller_;
  AppSettings settings_;
  SpeculativeImeState speculative_ime_;
};
