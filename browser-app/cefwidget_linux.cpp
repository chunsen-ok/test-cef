#include "cefwidget_linux.h"
#include <QUrl>
#include <QPainter>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QGuiApplication>

#include "simple_handler.h"

#if defined(CEF_X11)
#include <X11/Xlib.h>
#endif

class CefWidgetPrivate
{
    Q_DECLARE_PUBLIC(CefWidget)
    CefWidget* q_ptr;
public:
    CefWidgetPrivate(CefWidget* ptr)
        : q_ptr(ptr)
    {}

    QPointer<WrapWindow> wrap;
};

CefWidget::CefWidget(const QUrl& url, QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CefWidgetPrivate(this))
{
    Q_D(CefWidget);

    auto vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    auto w = new WrapWindow(url, nullptr);
    d->wrap = w;
    vlayout->addWidget(QWidget::createWindowContainer(w));
}

CefWidget::~CefWidget()
{
    Q_D(CefWidget);
}

void CefWidget::setUrl(const QUrl& url)
{
    Q_D(CefWidget);

    if (d->wrap) {
        d->wrap->setUrl(url);
    }
}

// void CefWidget::dropBrowser()
// {
//     Q_D(CefWidget);

//     if (auto browser = d->getBrowser()) {
//         qInfo() << "Close window";
//         browser->GetHost()->CloseBrowser(true);
//     }
// }

// bool CefWidget::event(QEvent* ev)
// {
//     return QWidget::event(ev);
// }

// void CefWidget::resizeEvent(QResizeEvent* ev)
// {
//     Q_D(CefWidget);

//     QWidget::resizeEvent(ev);

//     if (auto browser = d->getBrowser()) {
//         CefWindowHandle handle = browser->GetHost()->GetWindowHandle();
//         if (handle) {
// // #if defined(CEF_X11)
// //    Display *display = nullptr;
// //     // xcb_connection_t *connection = nullptr;
// //     // bool isPlatformX11 = false;
// //     if (auto *x11Application = qGuiApp->nativeInterface<QNativeInterface::QX11Application>()) {
// //         display = x11Application->display();
// //         // connection = x11Application->connection();
// //         // isPlatformX11 = true;
// //     }
      
// //     // 重新设置浏览器窗口的大小和位置
// //     int new_width = width();
// //     int new_height = height();
    
// //     if (display && new_width > 0 && new_height > 0) {
// //         qInfo() << "Resize window";
// //         // 调整浏览器窗口大小
// //         XResizeWindow(display, handle, new_width, new_height);
        
// //         // 强制重绘
// //         XClearArea(display, handle, 0, 0, new_width, new_height, True);
        
// //         // 确保更改生效
// //         XFlush(display);
        
// //         // 通知CEF主机大小已改变
// //         browser->GetHost()->WasResized();
// //     }
// // #endif
//         }
//     }
// }

// void CefWidget::showEvent(QShowEvent* ev)
// {
//     Q_D(CefWidget);

//     QWidget::showEvent(ev);

//     // if (!d->mBrowserCreated && isVisible()) {
//     //     createBrowser("about:blank");
//     // }
// }

// void CefWidget::closeEvent(QCloseEvent* ev)
// {
//     Q_D(CefWidget);

//     if (auto browser = d->getBrowser()) {
//         qInfo() << "Close window in closeEvent";
//         browser->GetHost()->CloseBrowser(true);
//     }

//     QWidget::closeEvent(ev);
// }

// void CefWidget::createBrowser(const QString& url)
// {
//     Q_D(CefWidget);

//     qInfo() << "browser created:" << d->mBrowserCreated;
//     if (d->mBrowserCreated)
//         return;

//     qInfo() << "is visible:" << isVisible();
//     if (!isVisible())
//         return;

//     d->handler = new SimpleHandler();
//     auto wrapperWindow = new WrapWindow(d->handler);

//     auto vlayout = new QVBoxLayout(this);
//     vlayout->setContentsMargins(0,0,0,0);
//     vlayout->addWidget(QWidget::createWindowContainer(wrapperWindow));
// }

WrapWindow::WrapWindow(const QUrl& url, QWindow* parent)
    : QWindow(parent)
    , mBackingStore(new QBackingStore(this))
{
    CefWindowInfo windowInfo;
    CefBrowserSettings browserSettings;
    browserSettings.background_color = CefColorSetARGB(0xff, 0xff, 0xff, 0xff);
    CefRect winRect(0, 0, width(), height());
    
    // 将cef界面嵌入qt界面中
    windowInfo.SetAsChild(winId(), winRect);
    CefBrowserHost::CreateBrowser(
        windowInfo, 
        SimpleHandler::GetInstance()->AsClient(), 
        url.toString().toStdString(),
        browserSettings, 
        nullptr, 
        nullptr
    );

    connect(SimpleHandler::GetInstance(), &SimpleHandler::browserCreated, this, [this]{ resizeBrowser(size()); });
}

WrapWindow::~WrapWindow()
{
    qInfo() << "~WrapWindow";
    if (SimpleHandler::GetInstance()) {
        if (auto browser = SimpleHandler::GetInstance()->GetBrowser()) {
            browser->GetHost()->CloseBrowser(true);
        }
    }
}

void WrapWindow::setUrl(const QUrl& url)
{
    if (auto handler = SimpleHandler::GetInstance()) {
        if (auto browser = handler->GetBrowser()) {
            browser->GetMainFrame()->LoadURL(url.toString().toStdString());
        }
    }
}

bool WrapWindow::event(QEvent* ev)
{
    qInfo() << ev->type();
    if (ev->type() == QEvent::UpdateRequest) {
        renderNow();
        return true;
    }

    return QWindow::event(ev);
}

void WrapWindow::exposeEvent(QExposeEvent* ev)
{
    if (isExposed())
        renderNow();
}

void WrapWindow::resizeEvent(QResizeEvent* ev)
{
    QWindow::resizeEvent(ev);

    mBackingStore->resize(ev->size());

    resizeBrowser(ev->size());
}

void WrapWindow::closeEvent(QCloseEvent* ev)
{
    QWindow::closeEvent(ev);
}

void WrapWindow::resizeBrowser(const QSize& size)
{
    if (auto browser = SimpleHandler::GetInstance()->GetBrowser()) {
        CefWindowHandle handle = browser->GetHost()->GetWindowHandle();
        if (handle) {
#if defined(CEF_X11)
   Display *display = nullptr;
    // xcb_connection_t *connection = nullptr;
    // bool isPlatformX11 = false;
    if (auto *x11Application = qGuiApp->nativeInterface<QNativeInterface::QX11Application>()) {
        display = x11Application->display();
        // connection = x11Application->connection();
        // isPlatformX11 = true;
    }
      
    // 重新设置浏览器窗口的大小和位置
    int new_width = size.width();
    int new_height = size.height();
    
    if (display && new_width > 0 && new_height > 0) {
        // 调整浏览器窗口大小
        XResizeWindow(display, handle, new_width, new_height);
        
        // 强制重绘
        XClearArea(display, handle, 0, 0, new_width, new_height, True);
        
        // 确保更改生效
        XFlush(display);
        
        // 通知CEF主机大小已改变
        browser->GetHost()->WasResized();
    }
#endif
        }
    }
}

void WrapWindow::renderNow()
{
    if (!isExposed())
        return;

    QRect rect(0, 0, width(), height());
    mBackingStore->beginPaint(rect);

    QPaintDevice *device = mBackingStore->paintDevice();
    QPainter painter(device);

    painter.fillRect(0, 0, width(), height(), Qt::white);
    painter.end();

    mBackingStore->endPaint();
    mBackingStore->flush(rect);
}
