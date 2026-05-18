#include "app.h"

#include "app_messages.h"
#include "resource.h"
#include "diagnostics.h"
#include "startup_registration.h"

#include <windowsx.h>

namespace {
constexpr wchar_t kWindowClassName[] = L"ImeKeysForUS.HiddenWindow";
constexpr UINT_PTR kDoubleTapTimerId = 1;
constexpr UINT kRefreshTrayMessage = WM_APP + 101;

UINT TaskbarCreatedMessageId() {
  static UINT message = RegisterWindowMessageW(L"TaskbarCreated");
  return message;
}
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

  hwnd_ =
      CreateWindowExW(WS_EX_TOOLWINDOW, kWindowClassName, L"IME Keys for US",
                      WS_OVERLAPPED, 0, 0, 0, 0, nullptr, nullptr, instance_,
                      this);
  if (hwnd_ == nullptr) {
    return FailStartup(L"CreateWindowExW failed.", GetLastError());
  }
  LogStartupEvent(L"Hidden window created.");

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

const wchar_t* App::WindowClassName() {
  return kWindowClassName;
}

UINT App::RefreshTrayMessageId() {
  return kRefreshTrayMessage;
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
  if (message == WM_APP_SPECULATIVE_IME_SET) {
    BeginSpeculativeImeSet(ImeGestureIdFromParam(wparam),
                           reinterpret_cast<HWND>(lparam),
                           ImeOpenFromParam(wparam));
    return 0;
  }

  if (message == WM_APP_SPECULATIVE_IME_RESTORE) {
    RestoreSpeculativeIme(static_cast<DWORD>(wparam));
    return 0;
  }

  if (message == WM_APP_SPECULATIVE_IME_COMMIT) {
    CommitSpeculativeIme(static_cast<DWORD>(wparam));
    return 0;
  }

  if (message == WM_APP_KEYBOARD_PENDING_CHANGED) {
    UpdateKeyboardTimer(hwnd);
    return 0;
  }

  if (message == TrayIcon::MessageId()) {
    UINT command = tray_icon_.HandleMessage(hwnd_, wparam, lparam);
    HandleCommand(command);
    return 0;
  }

  UINT taskbar_created_message = TaskbarCreatedMessageId();
  if (message == kRefreshTrayMessage ||
      (taskbar_created_message != 0 && message == taskbar_created_message)) {
    RefreshTrayIcon();
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
      if (HandleCommand(LOWORD(wparam))) {
        return 0;
      }
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProcW(hwnd, message, wparam, lparam);
}

bool App::HandleCommand(UINT command) {
  if (command == ID_TRAY_START_AT_SIGN_IN) {
    if (IsStartupEnabled()) {
      DisableStartup();
    } else {
      EnableStartup();
    }
    return true;
  }

  if (command == ID_TRAY_EXIT) {
    PostQuitMessage(0);
    return true;
  }

  return false;
}

void App::RefreshTrayIcon() {
  LogStartupEvent(L"Tray icon refresh attempted.");
  if (tray_icon_.Add(hwnd_)) {
    LogStartupEvent(L"Tray icon refreshed.");
  } else {
    LogLastError(L"Tray icon refresh failed.", tray_icon_.LastError());
  }
}

void App::UpdateKeyboardTimer(HWND hwnd) {
  KillTimer(hwnd, kDoubleTapTimerId);
  if (keyboard_hook_.HasPendingTap()) {
    SetTimer(hwnd, kDoubleTapTimerId, keyboard_hook_.PendingTapDelayMs(),
             nullptr);
  }
}

void App::BeginSpeculativeImeSet(DWORD gesture_id, HWND target_hwnd,
                                 bool open) {
  ImeController::Target target = ImeController::CaptureTarget(target_hwnd);

  SpeculativeImeState state{};
  state.gesture_id = gesture_id;
  state.target = target;
  state.can_restore = ime_controller_.GetOpenStatus(target, &state.original_open);

  if (state.can_restore) {
    speculative_ime_ = state;
  }
  ime_controller_.SetOpenStatus(target, open);
}

void App::RestoreSpeculativeIme(DWORD gesture_id) {
  if (speculative_ime_.gesture_id != gesture_id) {
    return;
  }

  if (speculative_ime_.can_restore) {
    ime_controller_.SetOpenStatus(speculative_ime_.target,
                                  speculative_ime_.original_open);
  }
  speculative_ime_ = {};
}

void App::CommitSpeculativeIme(DWORD gesture_id) {
  if (speculative_ime_.gesture_id == gesture_id) {
    speculative_ime_ = {};
  }
}
