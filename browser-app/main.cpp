// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <QDir>
#include <QApplication>
#include "mainwindow.h"

#if defined(Q_OS_LINUX)
#include "simple_app.h"

#if defined(CEF_X11)
#include <X11/Xlib.h>
#endif
#endif

#include <include/base/cef_logging.h>
#include <include/cef_command_line.h>

#if defined(Q_OS_LINUX)
#if defined(CEF_X11)
namespace {

int XErrorHandlerImpl(Display* display, XErrorEvent* event) {
  LOG(WARNING) << "X error received: " << "type " << event->type << ", "
               << "serial " << event->serial << ", " << "error_code "
               << static_cast<int>(event->error_code) << ", " << "request_code "
               << static_cast<int>(event->request_code) << ", " << "minor_code "
               << static_cast<int>(event->minor_code);
  return 0;
}


int XIOErrorHandlerImpl(Display* display) {
    return 0;
}

}  // namespace
#endif  // defined(CEF_X11)

// Entry point function for all processes.
NO_STACK_PROTECTOR
int main(int argc, char* argv[])
{
  // Provide CEF with command-line arguments.
  CefMainArgs main_args(argc, argv);

  // CEF applications have multiple sub-processes (render, GPU, etc) that share
  // the same executable. This function checks the command-line and, if this is
  // a sub-process, executes the appropriate logic.
  int exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
  if (exit_code >= 0) {
    // The sub-process has completed so return here.
    return exit_code;
  }

  QApplication a(argc, argv);

#if defined(CEF_X11)
  // Install xlib error handlers so that the application won't be terminated
  // on non-fatal errors.
  XSetErrorHandler(XErrorHandlerImpl);
  XSetIOErrorHandler(XIOErrorHandlerImpl);
#endif

  // Parse command-line arguments for use in this method.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromArgv(argc, argv);
    // 在命令行参数中添加
  command_line->AppendSwitch("--disable-gpu");
  command_line->AppendSwitch("--disable-gpu-compositing");


  // Specify CEF global settings here.
  CefSettings settings;
  settings.multi_threaded_message_loop = true;
  // 禁用无窗口渲染
  settings.windowless_rendering_enabled = false;  
  // 添加GPU相关设置
  settings.log_severity = LOGSEVERITY_DISABLE;  // 禁用详细日志
  CefString(&settings.root_cache_path).FromASCII("/home/zhizhi/projects/test-cef/build/browser-app-cache");

// When generating projects with CMake the CEF_USE_SANDBOX value will be defined
// automatically. Pass -DUSE_SANDBOX=OFF to the CMake command-line to disable
// use of the sandbox.
// #if !defined(CEF_USE_SANDBOX)
//   settings.no_sandbox = true;
// #endif
  
  // SimpleApp implements application-level callbacks for the browser process.
  // It will create the first browser instance in OnContextInitialized() after
  // CEF has initialized.
  CefRefPtr<SimpleApp> app(new SimpleApp);
  MainWindow w;

  QObject::connect(app.get(), &SimpleApp::contextInitialized, &w, &MainWindow::onContextInitialized);

  w.resize(800, 700);
  w.show();

  // Initialize the CEF browser process. May return false if initialization
  // fails or if early exit is desired (for example, due to process singleton
  // relaunch behavior).
  qInfo() << "Initializing CEF...";
  if (!CefInitialize(main_args, settings, app.get(), nullptr)) {
    qCritical() << "CEF initialization failed!";
    return CefGetExitCode();
  }
  qInfo() << "CEF initialized successfully";

  return a.exec();

  // Run the CEF message loop. This will block until CefQuitMessageLoop() is
  // called.
  // CefRunMessageLoop();
}

#endif // defined(Q_OS_LINUX)

#include <windows.h>

#include <include/cef_command_line.h>
#include <include/cef_sandbox_win.h>
#include <include/cef_version_info.h>
#include "simple_app.h"

namespace {

int RunMain(HINSTANCE hInstance, int argc, char* argv[], void* sandbox_info)
{
    int exit_code;

    // Provide CEF with command-line arguments.
    CefMainArgs main_args(hInstance);

    // CEF applications have multiple sub-processes (render, GPU, etc) that share
    // the same executable. This function checks the command-line and, if this is
    // a sub-process, executes the appropriate logic.
    exit_code = CefExecuteProcess(main_args, nullptr, sandbox_info);
    if (exit_code >= 0) {
        // The sub-process has completed so return here.
        return exit_code;
    }

    // Parse command-line arguments for use in this method.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());

    QApplication a(argc, argv);

    // Specify CEF global settings here.
    CefSettings settings;
    if (!sandbox_info) {
        settings.no_sandbox = true;
    }
    settings.multi_threaded_message_loop = true;
    // 禁用无窗口渲染
    settings.windowless_rendering_enabled = false;
    // 添加GPU相关设置
    settings.log_severity = LOGSEVERITY_DISABLE;  // 禁用详细日志

    QDir appDir{a.applicationDirPath()};
    const auto cachePath = appDir.absoluteFilePath("cef-cache");
    CefString(&settings.root_cache_path).FromString(cachePath.toStdString());

    // SimpleApp implements application-level callbacks for the browser process.
    // It will create the first browser instance in OnContextInitialized() after
    // CEF has initialized.
    CefRefPtr<SimpleApp> app(new SimpleApp);
    MainWindow w;

    QObject::connect(app.get(), &SimpleApp::contextInitialized, &w, &MainWindow::onContextInitialized);

    w.resize(800, 700);
    w.show();

    // Initialize the CEF browser process. May return false if initialization
    // fails or if early exit is desired (for example, due to process singleton
    // relaunch behavior).
    if (!CefInitialize(main_args, settings, app.get(), sandbox_info)) {
        return CefGetExitCode();
    }

    QObject::connect(&a, &QCoreApplication::aboutToQuit, &a, [](){
        qInfo() << "App quit";
        SimpleHandler::GetInstance()->CloseAllBrowsers(true);
        CefShutdown();
    });

    a.exec();

    return 0;
}

}  // namespace

#if defined(CEF_USE_BOOTSTRAP)

// Entry point called by bootstrap.exe when built as a DLL.
CEF_BOOTSTRAP_EXPORT int RunWinMain(HINSTANCE hInstance,
                                    LPTSTR lpCmdLine,
                                    int nCmdShow,
                                    void* sandbox_info,
                                    cef_version_info_t* /*version_info*/) {
    return ::RunMain(hInstance, lpCmdLine, nCmdShow, sandbox_info);
}

#else  // !defined(CEF_USE_BOOTSTRAP)


int main(int argc, char* argv[])
{
    QString path(argv[0]);
#ifdef UNICODE
    HINSTANCE hInstance = GetModuleHandle(path.toStdWString().c_str());
#else
    HINSTANCE hInstance = GetModuleHandle(path.toStdString().c_str());
#endif
    void* sandbox_info = nullptr;

#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    return ::RunMain(hInstance, argc, argv, sandbox_info);
}

#endif  // !defined(CEF_USE_BOOTSTRAP)
