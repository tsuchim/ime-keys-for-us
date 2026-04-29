#include "app.h"
#include "single_instance.h"
#include "startup_registration.h"

#include <windows.h>
#include <shellapi.h>

namespace {
bool HasCommandLineFlag(PWSTR command_line, const wchar_t* flag) {
  int argc = 0;
  PWSTR* argv = CommandLineToArgvW(command_line, &argc);
  if (argv == nullptr) {
    return false;
  }

  bool found = false;
  for (int i = 0; i < argc; ++i) {
    if (wcscmp(argv[i], flag) == 0) {
      found = true;
      break;
    }
  }

  LocalFree(argv);
  return found;
}
}  // namespace

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, PWSTR command_line, int) {
  if (HasCommandLineFlag(command_line, L"--enable-startup")) {
    return EnableStartup() ? 0 : 1;
  }

  if (HasCommandLineFlag(command_line, L"--disable-startup")) {
    return DisableStartup() ? 0 : 1;
  }

  SingleInstance single_instance(L"Local\\ImeKeysForUS.SingleInstance");
  if (!single_instance.IsPrimary()) {
    return 0;
  }

  App app(instance);
  if (!app.Initialize()) {
    return 1;
  }
  return app.Run();
}
