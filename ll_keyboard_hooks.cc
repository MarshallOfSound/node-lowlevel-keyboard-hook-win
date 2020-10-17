#include <napi.h>
#include <sstream>
#include <string>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#define _WIN32_WINNT 0x050

using namespace Napi;

ThreadSafeFunction tsfn;
std::thread nativeThread;
HHOOK hhkLowLevelKybd;
std::string str;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    auto callback = []( Napi::Env env, Function jsCallback, std::string *value) {
        jsCallback.Call({ String::New(env, *value )});
    };

    BOOL fEatKeystroke = FALSE;

    if (nCode == HC_ACTION)
    {
        std::ostringstream stream;
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        switch (wParam)
        {
        case WM_KEYDOWN:
            stream << p->vkCode;
            str = "down::" + stream.str();
            tsfn.NonBlockingCall(&str, callback);
            break;
        case WM_KEYUP:
            stream << p->vkCode;
            str = "up::" + stream.str();
            tsfn.NonBlockingCall(&str, callback);
            break;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void looper() {
    hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

    MSG msg;
    while (!GetMessage(&msg, NULL, NULL, NULL)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhkLowLevelKybd);

    tsfn.Release();
}

void RunCallback(const CallbackInfo& info) {
  Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsFunction()) {
      TypeError::New(env, "Function expected").ThrowAsJavaScriptException();
  }

  Function callback = info[0].As<Function>();

  tsfn = ThreadSafeFunction::New(
      env,
      callback,
      "asdasdasda", // Name?
      0,
      1,
      []( Napi::Env ) { // Finalizer used to clean threads up
          nativeThread.join();
      }
  );

  nativeThread = std::thread(looper);
  return;
}

Object Init(Env env, Object exports) {
  exports.Set("hookKeyboard", Function::New(env, RunCallback));
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
