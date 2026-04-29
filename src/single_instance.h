#pragma once

#include <windows.h>

class SingleInstance {
 public:
  explicit SingleInstance(const wchar_t* mutex_name);
  ~SingleInstance();

  bool IsPrimary() const;

 private:
  HANDLE mutex_ = nullptr;
  bool primary_ = false;
};

