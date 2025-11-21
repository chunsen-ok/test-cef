#include "cefwidget_win32.h"

class CefWidgetPrivate
{
    Q_DECLARE_PUBLIC(CefWidget)
    CefWidget* q_ptr;
public:
    CefWidgetPrivate(CefWidget* ptr)
        : q_ptr(ptr)
    {}
};

CefWidget::CefWidget(const QUrl& url, QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CefWidgetPrivate(this))
{
    Q_D(CefWidget);
}

CefWidget::~CefWidget()
{
    Q_D(CefWidget);
}

void CefWidget::setUrl(const QUrl& url)
{
    Q_D(CefWidget);
}
