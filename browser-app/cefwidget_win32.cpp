#include "cefwidget_win32.h"
#include <QUrl>
#include <QEvent>
#include <QDebug>
#include <include/cef_browser.h>
#include "simple_handler.h"

class CefWidgetPrivate
{
    Q_DECLARE_PUBLIC(CefWidget)
    CefWidget* q_ptr;
public:
    CefWidgetPrivate(CefWidget* ptr)
        : q_ptr(ptr)
    {}

    std::size_t browserId{0};
};

CefWidget::CefWidget(const QUrl& url, QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CefWidgetPrivate(this))
{
    Q_D(CefWidget);

    d->browserId = SimpleHandler::GetInstance()->CreateBrowserForWidget(this, url);
}

CefWidget::~CefWidget()
{
    Q_D(CefWidget);

    if (auto browser = SimpleHandler::GetInstance()->GetBrowser(d->browserId)) {
        browser->GetHost()->TryCloseBrowser();
    }
}

size_t CefWidget::browserId() const
{
    Q_D(const CefWidget);

    return d->browserId;
}

void CefWidget::setUrl(const QUrl& url)
{
    Q_D(CefWidget);

    if (auto browser = SimpleHandler::GetInstance()->GetBrowser(d->browserId)) {
        browser->GetMainFrame()->LoadURL(url.toString().toStdString());
    }
}

bool CefWidget::event(QEvent *ev)
{
    qDebug() << ev->type();
    if (ev->type() == QEvent::WinIdChange) {
        qDebug() << "winid change:" << winId();
    }

    return QWidget::event(ev);
}

void CefWidget::showEvent(QShowEvent *ev)
{
    QWidget::showEvent(ev);
}

void CefWidget::resizeEvent(QResizeEvent *ev)
{
    Q_D(CefWidget);

    QWidget::resizeEvent(ev);

    if (auto browser = SimpleHandler::GetInstance()->GetBrowser(d->browserId)) {
        auto wid = browser->GetHost()->GetWindowHandle();
        auto rect = this->geometry();

        SetWindowPos(wid, nullptr, 0, 0, rect.width() *devicePixelRatio(), rect.height()*devicePixelRatio(),
                     SWP_NOZORDER | SWP_NOACTIVATE);

        browser->GetHost()->WasResized();
    }
}
