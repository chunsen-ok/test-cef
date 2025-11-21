#include "mainwindow.h"
#include <QVBoxLayout>
#include <QGuiApplication>
#include <qnativeinterface.h>
#include <QResizeEvent>
#include <QShowEvent>
#include <QUrl>
#include <QTimer>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTabBar>

#include "cefwidget.h"
#include "simple_handler.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mTabWidget(new QTabWidget(this))
{
    setCentralWidget(mTabWidget);

    mTabWidget->addTab(new QWidget(mTabWidget), tr("Qt Widget"));
}

MainWindow::~MainWindow()
{

}

void MainWindow::onContextInitialized()
{
    auto body = new QWidget(mTabWidget);
    auto vlayout = new QVBoxLayout(body);
    vlayout->setContentsMargins(0,0,0,0);

    auto searchField = new QLineEdit(body);
    vlayout->addWidget(searchField);

    auto web = new CefWidget(QUrl{"https://www.bing.com"}, mTabWidget);
    body->setProperty("_browserId", web->browserId());
    vlayout->addWidget(web, 1);

    mTabWidget->addTab(body, tr("Cef"));

    connect(SimpleHandler::GetInstance(), &SimpleHandler::titleChanged, this, [this](std::size_t browserId, const QString& title){
        for (int i = 0; i < mTabWidget->count(); ++i) {
            auto browser = mTabWidget->widget(i)->property("_browserId").value<std::size_t>();
            if (browser == browserId) {
                auto tab = mTabWidget->tabBar();
                tab->setTabText(i, title);
            }
        }
    });

    connect(searchField, &QLineEdit::editingFinished, web, [web, searchField]{
        const auto url = searchField->text().trimmed();
        web->setUrl(QUrl{url});
    });
}

void MainWindow::closeEvent(QCloseEvent* ev)
{
    qInfo() << "Close MainWindow";
    QMainWindow::closeEvent(ev);
}
