#include "app.h"
#include "single_instance.h"

#include <windows.h>

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
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

