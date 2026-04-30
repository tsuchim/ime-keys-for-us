#include "app.h"

#include "app_messages.h"
#include "resource.h"
#include "diagnostics.h"
#include "startup_registration.h"

#include <windowsx.h>

namespace {
constexpr wchar_t kWindowClassName[] = L"ImeKeysForUS.HiddenWindow";
constexpr UINT_PTR kDoubleTapTimerId = 1;
}  // namespace

App::App(HINSTANCE instance) : instance_(instance), tray_icon_(instance) {}

App::~App() {
  keyboard_hook_.Uninstall();
  if (hwnd_ != nullptr) {
    KillTimer(hwnd_, kDoubleTapTimerId);
  }
  tray_icon_.Remove();
}

bool App::Initialize() {
  LogStartupEvent(L"App initialization started.");

  WNDCLASSEXW wc{};
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = App::WindowProc;
  wc.hInstance = instance_;
  wc.lpszClassName = kWindowClassName;

  if (RegisterClassExW(&wc) == 0) {
    return FailStartup(L"RegisterClassExW failed.", GetLastError());
  }
  LogStartupEvent(L"Window class registered.");

  hwnd_ = CreateWindowExW(0, kWindowClassName, L"IME Keys for US", 0, 0, 0, 0,
                          0, HWND_MESSAGE, nullptr, instance_, this);
  if (hwnd_ == nullptr) {
    return FailStartup(L"CreateWindowExW failed.", GetLastError());
  }
  LogStartupEvent(L"Message-only window created.");

  settings_ = LoadSettings();
  LogStartupEvent(L"Settings loaded.");
  keyboard_hook_.SetDoubleTapTimeout(settings_.alt_double_tap_ms);

  if (!keyboard_hook_.Install(hwnd_)) {
    return FailStartup(L"SetWindowsHookExW(WH_KEYBOARD_LL) failed.",
                       keyboard_hook_.LastError());
  }
  LogStartupEvent(L"Keyboard hook installed.");

  LogStartupEvent(L"Tray icon add attempted.");
  if (tray_icon_.Add(hwnd_)) {
    LogStartupEvent(L"Tray icon added.");
  } else {
    ShowTrayIconStartupWarning(tray_icon_.LastError());
  }
  return true;
}

int App::Run() {
  LogStartupEvent(L"App message loop entered.");
  MSG message{};
  while (GetMessageW(&message, nullptr, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
  return static_cast<int>(message.wParam);
}

bool App::FailStartup(const wchar_t* stage, DWORD error) {
  ShowFatalStartupError(stage, error);
  return false;
}

LRESULT CALLBACK App::WindowProc(HWND hwnd, UINT message, WPARAM wparam,
                                 LPARAM lparam) {
  App* app = reinterpret_cast<App*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
  if (message == WM_NCCREATE) {
    auto* create = reinterpret_cast<CREATESTRUCTW*>(lparam);
    app = reinterpret_cast<App*>(create->lpCreateParams);
    app->hwnd_ = hwnd;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
  }

  if (app != nullptr) {
    return app->HandleMessage(hwnd, message, wparam, lparam);
  }
  return DefWindowProcW(hwnd, message, wparam, lparam);
}

LRESULT App::HandleMessage(HWND hwnd, UINT message, WPARAM wparam,
                           LPARAM lparam) {
  if (message == WM_APP_SET_IME_OFF) {
    ime_controller_.SetOpenStatus(false);
    return 0;
  }

  if (message == WM_APP_SET_IME_ON) {
    ime_controller_.SetOpenStatus(true);
    return 0;
  }

  if (message == WM_APP_KEYBOARD_PENDING_CHANGED) {
    UpdateKeyboardTimer(hwnd);
    return 0;
  }

  if (message == TrayIcon::MessageId()) {
    tray_icon_.HandleMessage(hwnd_, lparam);
    return 0;
  }

  switch (message) {
    case WM_TIMER:
      if (wparam == kDoubleTapTimerId) {
        KillTimer(hwnd, kDoubleTapTimerId);
        keyboard_hook_.TickPendingTap();
        UpdateKeyboardTimer(hwnd);
      }
      return 0;
    case WM_COMMAND:
      if (LOWORD(wparam) == ID_TRAY_START_AT_SIGN_IN) {
        if (IsStartupEnabled()) {
          DisableStartup();
        } else {
          EnableStartup();
        }
        return 0;
      }
      if (LOWORD(wparam) == ID_TRAY_EXIT) {
        PostQuitMessage(0);
        return 0;
      }
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProcW(hwnd, message, wparam, lparam);
}

void App::UpdateKeyboardTimer(HWND hwnd) {
  KillTimer(hwnd, kDoubleTapTimerId);
  if (keyboard_hook_.HasPendingTap()) {
    SetTimer(hwnd, kDoubleTapTimerId, keyboard_hook_.PendingTapDelayMs(),
             nullptr);
  }
}
