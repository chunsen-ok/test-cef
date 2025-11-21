#ifndef CEFWIDGET_WIN32_H
#define CEFWIDGET_WIN32_H

#include <QWidget>

class CefWidgetPrivate;
class CefWidget: public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CefWidget)
public:
    explicit CefWidget(const QUrl& url, QWidget* parent = nullptr);
    ~CefWidget();

    void setUrl(const QUrl& url);

protected:
    bool event(QEvent* ev) override;
    void showEvent(QShowEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;

private:
    QScopedPointer<CefWidgetPrivate> d_ptr;
};

#endif
