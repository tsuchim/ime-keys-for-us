#include "tray_icon.h"

#include "resource.h"

namespace {
constexpr UINT kTrayIconId = 1;
constexpr UINT kTrayMessage = WM_APP + 100;
}  // namespace

TrayIcon::TrayIcon(HINSTANCE instance) : instance_(instance) {}

TrayIcon::~TrayIcon() {
  Remove();
}

UINT TrayIcon::MessageId() {
  return kTrayMessage;
}

bool TrayIcon::Add(HWND hwnd) {
  nid_ = {};
  nid_.cbSize = sizeof(nid_);
  nid_.hWnd = hwnd;
  nid_.uID = kTrayIconId;
  nid_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  nid_.uCallbackMessage = kTrayMessage;
  nid_.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
  wcscpy_s(nid_.szTip, L"IME Keys for US");

  added_ = Shell_NotifyIconW(NIM_ADD, &nid_) != FALSE;
  if (added_) {
    nid_.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &nid_);
  }
  return added_;
}

void TrayIcon::Remove() {
  if (added_) {
    Shell_NotifyIconW(NIM_DELETE, &nid_);
    added_ = false;
  }
}

void TrayIcon::HandleMessage(HWND hwnd, LPARAM lparam) {
  if (lparam == WM_CONTEXTMENU || lparam == WM_RBUTTONUP) {
    ShowMenu(hwnd);
  }
}

void TrayIcon::ShowMenu(HWND hwnd) {
  HMENU menu = CreatePopupMenu();
  if (menu == nullptr) {
    return;
  }

  AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"Exit");

  POINT cursor{};
  GetCursorPos(&cursor);
  SetForegroundWindow(hwnd);
  TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                 cursor.x, cursor.y, 0, hwnd, nullptr);
  PostMessageW(hwnd, WM_NULL, 0, 0);
  DestroyMenu(menu);
}
