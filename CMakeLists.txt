cmake_minimum_required(VERSION 3.21)

project(ImeKeysForUS VERSION 0.1.4 LANGUAGES CXX RC)

set(IME_KEYS_VERSION_4 "${PROJECT_VERSION}.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(IME_KEYS_UIACCESS "Embed the uiAccess=true manifest variant" OFF)
option(IME_KEYS_SIGN_AFTER_BUILD "Sign the executable after build using signtool" OFF)
set(IME_KEYS_SIGN_THUMBPRINT "A8B29BB0F03E4955B62E1CA798B6041F29812385" CACHE STRING "Code signing certificate thumbprint")
set(IME_KEYS_SIGN_TIMESTAMP_URL "http://timestamp.digicert.com" CACHE STRING "RFC3161 timestamp URL")

add_executable(ImeKeysForUS WIN32
  src/main.cpp
  src/app_messages.h
  src/app.cpp
  src/app.h
  src/keyboard_hook.cpp
  src/keyboard_hook.h
  src/settings.cpp
  src/settings.h
  src/diagnostics.cpp
  src/diagnostics.h
  src/startup_registration.cpp
  src/startup_registration.h
  src/ime_controller.cpp
  src/ime_controller.h
  src/tray_icon.cpp
  src/tray_icon.h
  src/single_instance.cpp
  src/single_instance.h
  src/synthetic_input_guard.cpp
  src/synthetic_input_guard.h
  src/resource.h
  src/app.rc
)

set_target_properties(ImeKeysForUS PROPERTIES
  OUTPUT_NAME "ime-keys-for-us"
)

target_compile_definitions(ImeKeysForUS PRIVATE
  UNICODE
  _UNICODE
  WIN32_LEAN_AND_MEAN
  NOMINMAX
  IME_KEYS_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  IME_KEYS_VERSION_MINOR=${PROJECT_VERSION_MINOR}
  IME_KEYS_VERSION_PATCH=${PROJECT_VERSION_PATCH}
  IME_KEYS_VERSION_STRING="${IME_KEYS_VERSION_4}"
)

target_link_libraries(ImeKeysForUS PRIVATE
  user32
  shell32
  ole32
  imm32
)

if(MSVC)
  target_compile_options(ImeKeysForUS PRIVATE /W4 /permissive-)
  set_property(TARGET ImeKeysForUS PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
  )
endif()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/app.manifest.in"
  "${CMAKE_CURRENT_BINARY_DIR}/generated/app.manifest"
  @ONLY
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/app-uiaccess.manifest.in"
  "${CMAKE_CURRENT_BINARY_DIR}/generated/app-uiaccess.manifest"
  @ONLY
)

set(_manifest "${CMAKE_CURRENT_BINARY_DIR}/generated/app.manifest")
if(IME_KEYS_UIACCESS)
  set(_manifest "${CMAKE_CURRENT_BINARY_DIR}/generated/app-uiaccess.manifest")
endif()

if(MSVC)
  target_link_options(ImeKeysForUS PRIVATE "/MANIFEST:EMBED" "/MANIFESTINPUT:${_manifest}")
endif()

if(IME_KEYS_SIGN_AFTER_BUILD)
  find_program(SIGNTOOL_EXE signtool)
  if(SIGNTOOL_EXE)
    add_custom_command(TARGET ImeKeysForUS POST_BUILD
      COMMAND "${SIGNTOOL_EXE}" sign
        /sha1 "${IME_KEYS_SIGN_THUMBPRINT}"
        /fd SHA256
        /tr "${IME_KEYS_SIGN_TIMESTAMP_URL}"
        /td SHA256
        "$<TARGET_FILE:ImeKeysForUS>"
      VERBATIM
    )
  else()
    message(WARNING "IME_KEYS_SIGN_AFTER_BUILD is ON, but signtool was not found. Build will continue unsigned.")
  endif()
endif()

install(TARGETS ImeKeysForUS RUNTIME DESTINATION .)
