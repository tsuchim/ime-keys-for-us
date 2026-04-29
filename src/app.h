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

 private:
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam,
                                     LPARAM lparam);
  LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam,
                        LPARAM lparam);

  HINSTANCE instance_;
  HWND hwnd_ = nullptr;
  TrayIcon tray_icon_;
  KeyboardHook keyboard_hook_;
  ImeController ime_controller_;
  AppSettings settings_;
};
