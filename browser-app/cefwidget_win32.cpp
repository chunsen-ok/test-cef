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
};

CefWidget::CefWidget(const QUrl& url, QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CefWidgetPrivate(this))
{
    Q_D(CefWidget);

    CefWindowInfo windowInfo;
    CefBrowserSettings browserSettings;
    browserSettings.background_color = CefColorSetARGB(0xff, 0xff, 0xff, 0xff);
    CefRect winRect(0, 0, width(), height());

    // 将cef界面嵌入qt界面中
    windowInfo.SetAsChild((HWND)winId(), winRect);
    CefBrowserHost::CreateBrowser(
        windowInfo,
        SimpleHandler::GetInstance()->AsClient(),
        url.toString().toStdString(),
        browserSettings,
        nullptr,
        nullptr
    );
}

CefWidget::~CefWidget()
{
    Q_D(CefWidget);

    if (auto browser = SimpleHandler::GetInstance()->GetBrowser()) {
        browser->GetHost()->TryCloseBrowser();
    }
}

void CefWidget::setUrl(const QUrl& url)
{
    Q_D(CefWidget);

    if (auto browser = SimpleHandler::GetInstance()->GetBrowser()) {
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
    QWidget::resizeEvent(ev);

    if (auto browser = SimpleHandler::GetInstance()->GetBrowser()) {
        auto wid = browser->GetHost()->GetWindowHandle();
        auto rect = this->geometry();

        SetWindowPos(wid, nullptr, 0, 0, rect.width() *devicePixelRatio(), rect.height()*devicePixelRatio(),
                     SWP_NOZORDER | SWP_NOACTIVATE);

        browser->GetHost()->WasResized();
    }
}
