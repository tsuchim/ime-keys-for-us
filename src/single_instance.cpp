#include "single_instance.h"

SingleInstance::SingleInstance(const wchar_t* mutex_name) {
  mutex_ = CreateMutexW(nullptr, TRUE, mutex_name);
  primary_ = mutex_ != nullptr && GetLastError() != ERROR_ALREADY_EXISTS;
}

SingleInstance::~SingleInstance() {
  if (mutex_ != nullptr) {
    if (primary_) {
      ReleaseMutex(mutex_);
    }
    CloseHandle(mutex_);
  }
}

bool SingleInstance::IsPrimary() const {
  return primary_;
}
