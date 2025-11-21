// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include <QObject>
#include "include/cef_app.h"
#include "simple_handler.h"

// Implement application-level callbacks for the browser process.
class SimpleApp : public QObject, public CefApp, public CefBrowserProcessHandler
{
  Q_OBJECT
public:
  explicit SimpleApp(QObject* parent = nullptr);

  // CefApp methods:
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }

  // CefBrowserProcessHandler methods:
  void OnContextInitialized() override;

signals:
  void contextInitialized(QPrivateSignal);

private:
  CefRefPtr<SimpleHandler> mHandler;
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleApp);
  DISALLOW_COPY_AND_ASSIGN(SimpleApp);
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
