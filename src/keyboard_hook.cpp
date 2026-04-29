#include "keyboard_hook.h"

#include "app_messages.h"
#include "synthetic_input_guard.h"

KeyboardHook* KeyboardHook::instance_ = nullptr;

KeyboardHook::KeyboardHook() = default;

KeyboardHook::~KeyboardHook() {
  Uninstall();
}

bool KeyboardHook::Install(HWND notify_window) {
  if (hook_ != nullptr) {
    return true;
  }

  notify_window_ = notify_window;
  instance_ = this;
  hook_ = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook::HookProc, nullptr, 0);
  if (hook_ == nullptr) {
    instance_ = nullptr;
    notify_window_ = nullptr;
    return false;
  }
  return true;
}

void KeyboardHook::Uninstall() {
  if (hook_ != nullptr) {
    UnhookWindowsHookEx(hook_);
    hook_ = nullptr;
  }
  if (instance_ == this) {
    instance_ = nullptr;
  }
  notify_window_ = nullptr;
}

void KeyboardHook::SetDoubleTapTimeout(DWORD timeout_ms) {
  double_tap_timeout_ms_ = timeout_ms;
}

void KeyboardHook::TickPendingTap() {
  if (gesture_.state == GestureState::Idle) {
    ResolveExpiredPendingTap(GetTickCount());
  }
}

bool KeyboardHook::HasPendingTap() const {
  return pending_tap_.key != AltKey::None;
}

DWORD KeyboardHook::PendingTapDelayMs() const {
  if (!HasPendingTap()) {
    return 0;
  }

  DWORD elapsed = GetTickCount() - pending_tap_.started_at;
  if (elapsed >= double_tap_timeout_ms_) {
    return 1;
  }
  return double_tap_timeout_ms_ - elapsed;
}

LRESULT CALLBACK KeyboardHook::HookProc(int code, WPARAM wparam,
                                        LPARAM lparam) {
  if (code < 0 || instance_ == nullptr) {
    return CallNextHookEx(nullptr, code, wparam, lparam);
  }

  const auto& event = *reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);
  LRESULT result = instance_->HandleKeyboardEvent(wparam, event);
  if (result != 0) {
    return result;
  }
  return CallNextHookEx(instance_->hook_, code, wparam, lparam);
}

LRESULT KeyboardHook::HandleKeyboardEvent(WPARAM wparam,
                                          const KBDLLHOOKSTRUCT& event) {
  if (SyntheticInputGuard::IsSynthetic(event)) {
    return 0;
  }

  AltKey alt_key = AltKey::None;
  const bool is_alt = IsAltKey(event, &alt_key);

  if (IsKeyDownMessage(wparam)) {
    return HandleKeyDown(event, is_alt ? alt_key : AltKey::None) ? 1 : 0;
  }

  if (IsKeyUpMessage(wparam) && is_alt) {
    return HandleKeyUp(event, alt_key) ? 1 : 0;
  }

  return 0;
}

bool KeyboardHook::HandleKeyDown(const KBDLLHOOKSTRUCT& event, AltKey key) {
  if (key == AltKey::None) {
    if (gesture_.state == GestureState::AltDownHeld &&
        gesture_.key != AltKey::None) {
      ClearPendingTap();
      ReplayAltDown(gesture_.key);
      gesture_.state = GestureState::NormalShortcut;
    }
    return false;
  }

  DWORD now = event.time != 0 ? event.time : GetTickCount();

  if (event.flags & LLKHF_UP) {
    return false;
  }

  if (gesture_.state == GestureState::Idle) {
    ResolveExpiredPendingTap(now);
    if (IsPendingTapSameKeyWithinTimeout(key, now)) {
      ClearPendingTap();
      ReplayAltDown(key);
      gesture_ = {};
      gesture_.key = key;
      gesture_.started_at = now;
      gesture_.state = GestureState::NormalShortcut;
      return true;
    }
    if (HasPendingTap() && pending_tap_.key != key) {
      ClearPendingTap();
    }
    BeginAltGesture(key, now);
    MarkConsumeUp(key);
    return true;
  }

  if (gesture_.state == GestureState::AltDownHeld &&
      gesture_.key != AltKey::None && gesture_.key != key) {
    ClearPendingTap();
    EmitStandaloneAlt(key);
    MarkConsumeUp(key);
    gesture_.state = GestureState::CrossFallback;
    return true;
  }

  if (gesture_.state == GestureState::CrossFallback) {
    return true;
  }

  return false;
}

bool KeyboardHook::HandleKeyUp(const KBDLLHOOKSTRUCT&, AltKey key) {
  if (gesture_.state == GestureState::Idle) {
    return false;
  }

  if (gesture_.state == GestureState::AltDownHeld && gesture_.key == key) {
    DWORD now = GetTickCount();
    ResolveExpiredPendingTap(now);
    BeginPendingTap(key, now);
    ResetGesture();
    return true;
  }

  if (gesture_.state == GestureState::NormalShortcut) {
    ResetGesture();
    return false;
  }

  if (gesture_.state == GestureState::CrossFallback) {
    if (ShouldConsumeUp(key)) {
      ClearConsumeUp(key);
    }
    if (!AnyConsumeUp()) {
      ResetGesture();
    }
    return true;
  }

  ResetGesture();
  return true;
}

void KeyboardHook::BeginAltGesture(AltKey key, DWORD timestamp) {
  gesture_ = {};
  gesture_.key = key;
  gesture_.started_at = timestamp;
  gesture_.state = GestureState::AltDownHeld;
}

void KeyboardHook::ResetGesture() {
  gesture_ = {};
}

void KeyboardHook::BeginPendingTap(AltKey key, DWORD timestamp) {
  pending_tap_.key = key;
  pending_tap_.started_at = timestamp;
  NotifyPendingTapChanged();
}

void KeyboardHook::ClearPendingTap() {
  if (!HasPendingTap()) {
    return;
  }
  pending_tap_ = {};
  NotifyPendingTapChanged();
}

void KeyboardHook::ResolveExpiredPendingTap(DWORD now) {
  if (IsPendingTapExpired(now)) {
    AltKey key = pending_tap_.key;
    ClearPendingTap();
    PostImeRequestForTap(key);
  }
}

void KeyboardHook::NotifyPendingTapChanged() {
  if (notify_window_ != nullptr) {
    PostMessageW(notify_window_, WM_APP_KEYBOARD_PENDING_CHANGED, 0, 0);
  }
}

bool KeyboardHook::IsPendingTapSameKeyWithinTimeout(AltKey key,
                                                   DWORD now) const {
  return pending_tap_.key == key && !IsPendingTapExpired(now);
}

bool KeyboardHook::IsPendingTapExpired(DWORD now) const {
  if (!HasPendingTap()) {
    return false;
  }
  return now - pending_tap_.started_at >= double_tap_timeout_ms_;
}

void KeyboardHook::ReplayAltDown(AltKey key) {
  INPUT input{};
  input.type = INPUT_KEYBOARD;
  input.ki.wScan = ScanCodeForAlt(key);
  input.ki.dwFlags = KEYEVENTF_SCANCODE | FlagsForAlt(key);
  input.ki.dwExtraInfo = SyntheticInputGuard::Marker();
  SendInput(1, &input, sizeof(input));
}

void KeyboardHook::ReplayAltUp(AltKey key) {
  INPUT input{};
  input.type = INPUT_KEYBOARD;
  input.ki.wScan = ScanCodeForAlt(key);
  input.ki.dwFlags = KEYEVENTF_SCANCODE | FlagsForAlt(key) | KEYEVENTF_KEYUP;
  input.ki.dwExtraInfo = SyntheticInputGuard::Marker();
  SendInput(1, &input, sizeof(input));
}

void KeyboardHook::EmitStandaloneAlt(AltKey key) {
  ReplayAltDown(key);
  ReplayAltUp(key);
}

void KeyboardHook::PostImeRequestForTap(AltKey key) {
  if (notify_window_ == nullptr) {
    return;
  }

  if (key == AltKey::Left) {
    PostMessageW(notify_window_, WM_APP_SET_IME_OFF, 0, 0);
  } else if (key == AltKey::Right) {
    PostMessageW(notify_window_, WM_APP_SET_IME_ON, 0, 0);
  }
}

void KeyboardHook::MarkConsumeUp(AltKey key) {
  if (key == AltKey::Left) {
    gesture_.consume_left_up = true;
  } else if (key == AltKey::Right) {
    gesture_.consume_right_up = true;
  }
}

void KeyboardHook::ClearConsumeUp(AltKey key) {
  if (key == AltKey::Left) {
    gesture_.consume_left_up = false;
  } else if (key == AltKey::Right) {
    gesture_.consume_right_up = false;
  }
}

bool KeyboardHook::ShouldConsumeUp(AltKey key) const {
  if (key == AltKey::Left) {
    return gesture_.consume_left_up;
  }
  if (key == AltKey::Right) {
    return gesture_.consume_right_up;
  }
  return false;
}

bool KeyboardHook::AnyConsumeUp() const {
  return gesture_.consume_left_up || gesture_.consume_right_up;
}

bool KeyboardHook::IsKeyDownMessage(WPARAM wparam) {
  return wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN;
}

bool KeyboardHook::IsKeyUpMessage(WPARAM wparam) {
  return wparam == WM_KEYUP || wparam == WM_SYSKEYUP;
}

bool KeyboardHook::IsAltKey(const KBDLLHOOKSTRUCT& event, AltKey* key) {
  if (event.vkCode != VK_MENU && event.vkCode != VK_LMENU &&
      event.vkCode != VK_RMENU) {
    return false;
  }

  if (event.vkCode == VK_LMENU) {
    *key = AltKey::Left;
    return true;
  }

  if (event.vkCode == VK_RMENU) {
    *key = AltKey::Right;
    return true;
  }

  *key = (event.flags & LLKHF_EXTENDED) ? AltKey::Right : AltKey::Left;
  return true;
}

WORD KeyboardHook::ScanCodeForAlt(AltKey key) {
  const UINT vk = key == AltKey::Right ? VK_RMENU : VK_LMENU;
  return static_cast<WORD>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC));
}

DWORD KeyboardHook::FlagsForAlt(AltKey key) {
  return key == AltKey::Right ? KEYEVENTF_EXTENDEDKEY : 0;
}
