#include "synthetic_input_guard.h"

ULONG_PTR SyntheticInputGuard::Marker() {
  return static_cast<ULONG_PTR>(0x494D454B45595355ULL);
}

bool SyntheticInputGuard::IsSynthetic(const KBDLLHOOKSTRUCT& event) {
  return event.dwExtraInfo == Marker();
}

