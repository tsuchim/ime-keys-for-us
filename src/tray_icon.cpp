#include "tray_icon.h"

#include "resource.h"
#include "startup_registration.h"

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
  Remove();

  icon_ = static_cast<HICON>(
      LoadImageW(instance_, MAKEINTRESOURCEW(IDI_APP_ICON), IMAGE_ICON,
                 GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                 LR_DEFAULTCOLOR));
  owns_icon_ = icon_ != nullptr;
  if (icon_ == nullptr) {
    icon_ = LoadIconW(nullptr, IDI_APPLICATION);
  }

  nid_ = {};
  nid_.cbSize = sizeof(nid_);
  nid_.hWnd = hwnd;
  nid_.uID = kTrayIconId;
  nid_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  nid_.uCallbackMessage = kTrayMessage;
  nid_.hIcon = icon_;
  wcscpy_s(nid_.szTip, L"IME Keys for US");

  added_ = Shell_NotifyIconW(NIM_ADD, &nid_) != FALSE;
  last_error_ = added_ ? ERROR_SUCCESS : GetLastError();
  if (added_) {
    nid_.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &nid_);
  }
  return added_;
}

DWORD TrayIcon::LastError() const {
  return last_error_;
}

void TrayIcon::Remove() {
  if (added_) {
    Shell_NotifyIconW(NIM_DELETE, &nid_);
    added_ = false;
  }
  if (icon_ != nullptr && owns_icon_) {
    DestroyIcon(icon_);
  }
  icon_ = nullptr;
  owns_icon_ = false;
}

void TrayIcon::HandleMessage(HWND hwnd, LPARAM lparam) {
  UINT message = LOWORD(lparam);
  if (message == WM_CONTEXTMENU || message == WM_RBUTTONUP ||
      lparam == WM_CONTEXTMENU || lparam == WM_RBUTTONUP) {
    ShowMenu(hwnd);
  }
}

void TrayIcon::ShowMenu(HWND hwnd) {
  HMENU menu = CreatePopupMenu();
  if (menu == nullptr) {
    return;
  }

  AppendMenuW(menu, MF_STRING | MF_DISABLED, 0, L"IME Keys for US");
  AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
  UINT startup_flags = MF_STRING;
  if (IsStartupEnabled()) {
    startup_flags |= MF_CHECKED;
  }
  AppendMenuW(menu, startup_flags, ID_TRAY_START_AT_SIGN_IN,
              L"Start at sign-in");
  AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"Exit");

  POINT cursor{};
  GetCursorPos(&cursor);
  SetForegroundWindow(hwnd);
  TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                 cursor.x, cursor.y, 0, hwnd, nullptr);
  PostMessageW(hwnd, WM_NULL, 0, 0);
  DestroyMenu(menu);
}
