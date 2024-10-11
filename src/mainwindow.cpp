
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
#include "include/mainwindow.h"

AppWindow::AppWindow() {
    // Set the window flags to remove the title bar
    setWindowFlags(Qt::FramelessWindowHint);

    // Set the window's size
    resize(800, 600);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(1, 0, 1, 0);
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
    // auto dev_view = new QWebEngineView();
    // mainLayout->addWidget(dev_view);
    // webControls->page()->setDevToolsPage(dev_view->page());

    // setting up web controls for window
    QWebChannel *channel = new QWebChannel(webControls);
    ClickHandler *handler = new ClickHandler();
    channel->registerObject(QStringLiteral("clickHandler"), handler);
    webControls->page()->setWebChannel(channel);

    // connecting js web controls to qt
    connect(handler, &ClickHandler::closeRequested, this, &AppWindow::closeWindow);
    connect(handler, &ClickHandler::maximizeRequested, this, &AppWindow::toggleMaximizeRestore);
    connect(handler, &ClickHandler::minimizeRequested, this, &AppWindow::minimizeWindow);

    connect(handler, &ClickHandler::startMoveEvent, this, &AppWindow::startMoveEvent);

    connect(handler, &ClickHandler::createTabEvent, this, &AppWindow::createTab);
    connect(handler, &ClickHandler::tabChangeEvent, this, &AppWindow::changeTab);
    connect(handler, &ClickHandler::tabCloseEvent, this, &AppWindow::closeTab);

    // connect(addressBar, &QLineEdit::returnPressed, this, &AppWindow::loadPage);
    connect(handler, &ClickHandler::backRequested, this, &AppWindow::goBack);
    connect(handler, &ClickHandler::forwardRequested, this, &AppWindow::goForward);
    connect(handler, &ClickHandler::reloadRequested, this, &AppWindow::reload);
    connect(handler, &ClickHandler::urlChangeRequested, this, &AppWindow::changeUrl);
    // connect(reloadButton, &QPushButton::clicked, this, &AppWindow::reloadPage);
    // connect(tabWidget, &QTabWidget::currentChanged, this, &AppWindow::updateAddressBar);
    // connect(tabWidget->tabBar(), &QTabBar::tabCloseRequested, this, &AppWindow::closeTab);

    // setting up shortcuts
    connect(new QShortcut(QKeySequence("Ctrl+T"), this), &QShortcut::activated, this, &AppWindow::requestNewTab);
    connect(new QShortcut(QKeySequence("Ctrl+W"), this), &QShortcut::activated, this, &AppWindow::closeCurrentTab);
    connect(new QShortcut(QKeySequence("Ctrl+Tab"), this), &QShortcut::activated, this, &AppWindow::nextTab);
    connect(new QShortcut(QKeySequence("Ctrl+Shift+Tab"), this), &QShortcut::activated, this, &AppWindow::prevTab);
    connect(new QShortcut(QKeySequence("Ctrl+D"), this), &QShortcut::activated, this, &AppWindow::favDialog);


    // Add a context menu to create new tabs
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &AppWindow::customContextMenuRequested, this, &AppWindow::showContextMenu);

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
    // webControls->page()->runJavaScript("newTabElement.click();");
    // createTab();
}
