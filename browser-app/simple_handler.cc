// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_handler.h"

#include <QWidget>

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

    CloseAllBrowsers(true);
}

// static
SimpleHandler* SimpleHandler::GetInstance()
{
    return g_instance;
}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    std::lock_guard<std::mutex> lock{mMtx};
    if (!mPenddings.empty()) {
        auto id = mPenddings.front();
        mPenddings.erase(mPenddings.begin());

        mBrowserIndexes.insert(std::make_pair(browser->GetIdentifier(), id));
        mBrowsers.insert(std::make_pair(id, browser));

        emit browserCreated(id, QPrivateSignal{});
    } else {
        // ...
    }
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
    auto id = browser->GetIdentifier();
    if (auto it = mBrowserIndexes.find(id); it != mBrowserIndexes.end()) {
        emit titleChanged(it->second, QString::fromStdString(title.ToString()), QPrivateSignal{});
    }
}

size_t SimpleHandler::CreateBrowserForWidget(QWidget *widget, const QUrl& url)
{
    std::lock_guard<std::mutex> lock{mMtx};
    auto id = ++mBrowserId;
    mPenddings.push_back(id);

    const auto size = widget->size();
    CefWindowInfo windowInfo;
    CefBrowserSettings browserSettings;
    browserSettings.background_color = CefColorSetARGB(0xff, 0xff, 0xff, 0xff);
    CefRect winRect(0, 0, size.width(), size.height());

    // 将cef界面嵌入qt界面中
    windowInfo.SetAsChild((HWND)widget->winId(), winRect);
    CefBrowserHost::CreateBrowser(
        windowInfo,
        SimpleHandler::GetInstance()->AsClient(),
        url.toString().toStdString(),
        browserSettings,
        nullptr,
        nullptr
    );

    return id;
}

void SimpleHandler::ShowMainWindow()
{
}

void SimpleHandler::CloseAllBrowsers(bool force_close)
{
    std::lock_guard<std::mutex> lock{mMtx};
    for (auto& pair: mBrowsers) {
        if (pair.second) {
            pair.second->GetHost()->CloseBrowser(force_close);
            pair.second.reset();
        }
    }

    mBrowsers.clear();
    mBrowserIndexes.clear();
}

void SimpleHandler::CloseBrowser(size_t browserId, bool force)
{
    std::lock_guard<std::mutex> lock{mMtx};
    if (auto it = mBrowsers.find(browserId); it != mBrowsers.end()) {
        auto id = it->second->GetIdentifier();
        it->second->GetHost()->CloseBrowser(force);
        mBrowsers.erase(it);
        mBrowserIndexes.erase(id);
    }
}

CefRefPtr<CefBrowser> SimpleHandler::GetBrowser(size_t browserId) const
{
    if (auto it = mBrowsers.find(browserId); it != mBrowsers.end()) {
        return it->second;
    }
    return nullptr;
}
