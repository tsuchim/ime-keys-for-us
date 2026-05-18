#pragma once

#include <windows.h>

class ImeController {
 public:
  struct Target {
    HWND hwnd = nullptr;
    DWORD thread_id = 0;
    DWORD process_id = 0;
  };

  static Target CaptureForegroundTarget();

  bool SetOpenStatus(bool open) const;
  bool GetOpenStatus(const Target& target, bool* open) const;
  bool SetOpenStatus(const Target& target, bool open) const;
  bool IsSameTarget(const Target& target) const;
};