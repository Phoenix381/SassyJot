
#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QWebChannel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QUrl>

#include <QShortcut>

#include <QDir>
#include <QDirIterator>

#include <iostream>
#include "include/app_window.h"


// =============================================================================
// app window initialization
// =============================================================================

AppWindow::AppWindow() {
    // db api
    db = new DBController();
    
    // Set the window flags to remove the title bar
    setWindowFlags(Qt::FramelessWindowHint);

    // Set the window's size
    resize(800, 600);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // Controls widget
    webControls = new QWebEngineView();
    mainLayout->addWidget(webControls);
    webControls->load(QUrl("qrc:/html/controls.html"));
    // webControls->setStyleSheet("QWebEngineView { padding: 0px; margin: 0px; }");
    webControls->setFixedHeight(84);
    webControls->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // Tab widget
    tabWidget = new QTabWidget();
    mainLayout->addWidget(tabWidget);
    tabWidget->tabBar()->setVisible(false);

    // Developer tools
    auto dev_view = new QWebEngineView();
    mainLayout->addWidget(dev_view);
    webControls->page()->setDevToolsPage(dev_view->page());

    // setting up web controls for window
    QWebChannel *channel = new QWebChannel(webControls);
    handler = new ClickHandler();

    use this instead of handler
    channel->registerObject(QStringLiteral("clickHandler"), handler);
    webControls->page()->setWebChannel(channel);

    registerEvents();
    registerHotkeys();

    // Set layout in QWidget
    QWidget *window = new QWidget();
    // remove window borders
    window->setStyleSheet("QWidget { padding: 0px; margin: 0px; }");
    window->setLayout(mainLayout);

    // Set QWidget as the central layout of the main window
    this->setCentralWidget(window);

    // Create initial tab with QWebEngineView
    connect(webControls, &QWebEngineView::loadFinished, this, 
        [this](){this->webControls->page()->runJavaScript("newTabElement.click();");}
    );
}
