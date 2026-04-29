#pragma once

#include <windows.h>
#include <shellapi.h>

class TrayIcon {
 public:
  explicit TrayIcon(HINSTANCE instance);
  ~TrayIcon();

  bool Add(HWND hwnd);
  void Remove();
  void HandleMessage(HWND hwnd, LPARAM lparam);

  static UINT MessageId();

 private:
  void ShowMenu(HWND hwnd);

  HINSTANCE instance_;
  NOTIFYICONDATAW nid_{};
  bool added_ = false;
};

