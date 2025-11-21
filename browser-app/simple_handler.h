// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

#include <QObject>
#include <QUrl>
#include <mutex>

#include "include/cef_client.h"

class SimpleHandler : public QObject
    , public CefClient
    , public CefDisplayHandler
    , public CefLifeSpanHandler
{
    Q_OBJECT
public:
    explicit SimpleHandler();
    ~SimpleHandler() override;

    // Provide access to the single global instance of this object.
    // Used by platform entry points (cefsimple_mac.mm, etc.)
    static SimpleHandler* GetInstance();

    CefRefPtr<CefClient> AsClient() { return this; }

    // CefClient methods:
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

    // CefLifeSpanHandler methods:
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefDisplayHandler methods:
    void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

    std::size_t CreateBrowserForWidget(QWidget* widget, const QUrl& url = QUrl{"about:blank"});

    // Request that all existing browser windows close.
    void CloseAllBrowsers(bool force_close);
    void CloseBrowser(std::size_t browserId, bool force = false);
    CefRefPtr<CefBrowser> GetBrowser(std::size_t browserId) const;
    bool IsClosing() const { return false; }
    void ShowMainWindow();

signals:
    void browserCreated(std::size_t id, QPrivateSignal);
    void titleChanged(std::size_t id, const QString& title, QPrivateSignal);

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SimpleHandler);
    // Prevents copying (CEF objects shouldn't be copied).
    DISALLOW_COPY_AND_ASSIGN(SimpleHandler);

    std::mutex mMtx;
    std::size_t mBrowserId;
    std::vector<std::size_t> mPenddings;
    std::map<int, std::size_t> mBrowserIndexes;
    std::map<std::size_t, CefRefPtr<CefBrowser>> mBrowsers;
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
