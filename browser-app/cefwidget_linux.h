#ifndef BROWSER_APP_CEF_WIDGET_LINUX_H
#define BROWSER_APP_CEF_WIDGET_LINUX_H

#include <QWidget>
#include <QWindow>
#include <QBackingStore>

class CefWidgetPrivate;
class CefWidget: public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CefWidget)
public:
    explicit CefWidget(const QUrl& url, QWidget* parent = nullptr);
    ~CefWidget();

    void setUrl(const QUrl& url);

// protected:
//     bool event(QEvent* ev) override;
//     void resizeEvent(QResizeEvent *event) override;
//     void showEvent(QShowEvent *event) override;
//     void closeEvent(QCloseEvent* event) override;

private:
    QScopedPointer<CefWidgetPrivate> d_ptr;
};

class WrapWindow: public QWindow
{
    Q_OBJECT
public:
    explicit WrapWindow(const QUrl& url, QWindow* parent = nullptr);
    ~WrapWindow();

    void setUrl(const QUrl& url);

protected:
    bool event(QEvent* ev) override;
    void exposeEvent(QExposeEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;
    void closeEvent(QCloseEvent* ev) override;

protected slots:
    void resizeBrowser(const QSize& size);
    void renderNow();

private:
    QScopedPointer<QBackingStore> mBackingStore;
};

#endif
