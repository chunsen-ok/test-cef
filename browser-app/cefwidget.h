#ifndef BROWSER_APP_CEF_WIDGET_H
#define BROWSER_APP_CEF_WIDGET_H

#include <QtGlobal>

#if defined(Q_OS_LINUX)
#include "cefwidget_linux.h"
#elif defined(Q_OS_WIN)
#include "cefwidget_win32.h"
#endif

#endif
