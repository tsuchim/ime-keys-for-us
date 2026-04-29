#include "app.h"

#include "app_messages.h"
#include "resource.h"

#include <windowsx.h>

namespace {
constexpr wchar_t kWindowClassName[] = L"ImeKeysForUS.HiddenWindow";
constexpr UINT_PTR kLongPressTimerId = 1;
constexpr UINT kTimerIntervalMs = 20;
}  // namespace

App::App(HINSTANCE instance) : instance_(instance), tray_icon_(instance) {}

App::~App() {
  keyboard_hook_.Uninstall();
  if (hwnd_ != nullptr) {
    KillTimer(hwnd_, kLongPressTimerId);
  }
  tray_icon_.Remove();
}

bool App::Initialize() {
  WNDCLASSEXW wc{};
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = App::WindowProc;
  wc.hInstance = instance_;
  wc.lpszClassName = kWindowClassName;

  if (RegisterClassExW(&wc) == 0) {
    return false;
  }

  hwnd_ = CreateWindowExW(0, kWindowClassName, L"IME Keys for US", 0, 0, 0, 0,
                          0, HWND_MESSAGE, nullptr, instance_, this);
  if (hwnd_ == nullptr) {
    return false;
  }

  if (!keyboard_hook_.Install(hwnd_)) {
    return false;
  }

  SetTimer(hwnd_, kLongPressTimerId, kTimerIntervalMs, nullptr);
  tray_icon_.Add(hwnd_);
  return true;
}

int App::Run() {
  MSG message{};
  while (GetMessageW(&message, nullptr, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
  return static_cast<int>(message.wParam);
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

  if (message == TrayIcon::MessageId()) {
    tray_icon_.HandleMessage(hwnd_, lparam);
    return 0;
  }

  switch (message) {
    case WM_TIMER:
      if (wparam == kLongPressTimerId) {
        keyboard_hook_.TickLongPress();
      }
      return 0;
    case WM_COMMAND:
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
