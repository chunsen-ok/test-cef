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

private:
    QScopedPointer<CefWidgetPrivate> d_ptr;
};

#endif
