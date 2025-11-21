// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_handler.h"

#include <sstream>
#include <string>

#include "include/base/cef_callback.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

namespace {

// Anonymous namespace - variables here are only visible in this file
SimpleHandler* g_instance = nullptr;

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type) {
  return "data:" + mime_type + ";base64," +
         CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
             .ToString();
}

}  // namespace

SimpleHandler::SimpleHandler()
  : QObject()
{
  DCHECK(!g_instance);  // Debug assertion - crashes if instance already exists
  g_instance = this;
}

SimpleHandler::~SimpleHandler()
{
   g_instance = nullptr;

    if (mBrowser) {
        // 在析构时关闭浏览器
        mBrowser->GetHost()->CloseBrowser(true);
        mBrowser = nullptr;
    }
}

// static
SimpleHandler* SimpleHandler::GetInstance()
{
  return g_instance;
}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
  mBrowser = browser;
  emit browserCreated(QPrivateSignal{});
}
  
void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // All browser windows have closed. Quit the application message loop.
  // CefQuitMessageLoop();
}

// CefDisplayHandler methods:
void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
  if (mBrowser->IsSame(browser)) {
    emit titleChanged(QString::fromStdString(title.ToString()), QPrivateSignal{});
  }
}

void SimpleHandler::ShowMainWindow()
{
}

void SimpleHandler::CloseAllBrowsers(bool force_close)
{
}

CefRefPtr<CefBrowser> SimpleHandler::GetBrowser()
{
  return mBrowser;
}
