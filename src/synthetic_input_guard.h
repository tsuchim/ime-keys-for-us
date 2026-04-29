#pragma once

#include <windows.h>

class SyntheticInputGuard {
 public:
  static ULONG_PTR Marker();
  static bool IsSynthetic(const KBDLLHOOKSTRUCT& event);
};

