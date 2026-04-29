#pragma once

#include <windows.h>

class KeyboardHook {
 public:
  KeyboardHook();
  ~KeyboardHook();

  bool Install(HWND notify_window);
  void Uninstall();
  void SetDoubleTapTimeout(DWORD timeout_ms);
  void TickPendingTap();
  bool HasPendingTap() const;
  DWORD PendingTapDelayMs() const;

 private:
  enum class AltKey {
    None,
    Left,
    Right,
  };

  enum class GestureState {
    Idle,
    AltDownHeld,
    NormalShortcut,
    CrossFallback,
  };

  struct Gesture {
    AltKey key = AltKey::None;
    DWORD started_at = 0;
    GestureState state = GestureState::Idle;
    bool consume_left_up = false;
    bool consume_right_up = false;
  };

  struct PendingTap {
    AltKey key = AltKey::None;
    DWORD started_at = 0;
  };

  static LRESULT CALLBACK HookProc(int code, WPARAM wparam, LPARAM lparam);
  static KeyboardHook* instance_;

  LRESULT HandleKeyboardEvent(WPARAM wparam, const KBDLLHOOKSTRUCT& event);
  bool HandleKeyDown(const KBDLLHOOKSTRUCT& event, AltKey key);
  bool HandleKeyUp(const KBDLLHOOKSTRUCT& event, AltKey key);

  void BeginAltGesture(AltKey key, DWORD timestamp);
  void ResetGesture();
  void BeginPendingTap(AltKey key, DWORD timestamp);
  void ClearPendingTap();
  void ResolveExpiredPendingTap(DWORD now);
  void NotifyPendingTapChanged();
  bool IsPendingTapSameKeyWithinTimeout(AltKey key, DWORD now) const;
  bool IsPendingTapExpired(DWORD now) const;
  void ReplayAltDown(AltKey key);
  void ReplayAltUp(AltKey key);
  void EmitStandaloneAlt(AltKey key);
  void PostImeRequestForTap(AltKey key);
  void MarkConsumeUp(AltKey key);
  void ClearConsumeUp(AltKey key);
  bool ShouldConsumeUp(AltKey key) const;
  bool AnyConsumeUp() const;

  static bool IsKeyDownMessage(WPARAM wparam);
  static bool IsKeyUpMessage(WPARAM wparam);
  static bool IsAltKey(const KBDLLHOOKSTRUCT& event, AltKey* key);
  static WORD ScanCodeForAlt(AltKey key);
  static DWORD FlagsForAlt(AltKey key);

  HHOOK hook_ = nullptr;
  HWND notify_window_ = nullptr;
  Gesture gesture_;
  PendingTap pending_tap_;
  DWORD double_tap_timeout_ms_ = 200;
};
