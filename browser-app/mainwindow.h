#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class MainWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void onContextInitialized();

protected:
    void closeEvent(QCloseEvent* ev) override;

private:
    QTabWidget* mTabWidget;
};

#endif
