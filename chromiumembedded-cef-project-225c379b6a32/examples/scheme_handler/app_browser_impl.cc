// Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <sstream>

#include "examples/shared/app_factory.h"

#include "examples/minimal/client_minimal.h"
#include "examples/scheme_handler/scheme_handler_impl.h"
#include "examples/scheme_handler/scheme_strings.h"
#include "examples/shared/browser_util.h"
#include "examples/shared/resource_util.h"

namespace scheme_handler {

namespace {

std::string GetStartupURL() {
  std::stringstream ss;
  ss << kScheme << "://" << kDomain << "/" << kFileName;
  return ss.str();
}

}  // namespace

// Implementation of CefApp for the browser process.
class BrowserApp : public CefApp, public CefBrowserProcessHandler {
 public:
  BrowserApp() {}

  // CefApp methods:
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
    return this;
  }

  void OnBeforeCommandLineProcessing(
      const CefString& process_type,
      CefRefPtr<CefCommandLine> command_line) override {
    // Command-line flags can be modified in this callback.
    // |process_type| is empty for the browser process.
    if (process_type.empty()) {
#if defined(OS_MACOSX)
      // Disable the macOS keychain prompt. Cookies will not be encrypted.
      command_line->AppendSwitch("use-mock-keychain");
#endif
    }
  }

  void OnRegisterCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar) override {
    // Register the custom scheme as standard and secure.
    // Must be the same implementation in all processes.
    registrar->AddCustomScheme(kScheme, kSchemeRegistrationOptions);
  }

  // CefBrowserProcessHandler methods:
  void OnContextInitialized() override {
    // Register the custom scheme handler factory.
    RegisterSchemeHandlerFactory();

    // Create the browser window.
    shared::CreateBrowser(new minimal::Client(), GetStartupURL(),
                          CefBrowserSettings());
  }

 private:
  IMPLEMENT_REFCOUNTING(BrowserApp);
  DISALLOW_COPY_AND_ASSIGN(BrowserApp);
};

}  // namespace scheme_handler

namespace shared {

CefRefPtr<CefApp> CreateBrowserProcessApp() {
  return new scheme_handler::BrowserApp();
}

}  // namespace shared
