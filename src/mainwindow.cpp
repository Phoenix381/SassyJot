
#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QWebChannel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QUrl>

#include <QDir>
#include <QDirIterator>

#include <iostream>
#include "include/mainwindow.h"

MyWindow::MyWindow() {
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

    // Create initial tab with QWebEngineView
    createTab(tabWidget);
    

    // setting up web controls for window
    QWebChannel *channel = new QWebChannel(webControls);
    ClickHandler *handler = new ClickHandler();
    channel->registerObject(QStringLiteral("clickHandler"), handler);
    webControls->page()->setWebChannel(channel);

    // connecting js web controls to qt
    connect(handler, &ClickHandler::closeRequested, this, &MyWindow::closeWindow);
    connect(handler, &ClickHandler::maximizeRequested, this, &MyWindow::toggleMaximizeRestore);
    connect(handler, &ClickHandler::minimizeRequested, this, &MyWindow::minimizeWindow);

    connect(handler, &ClickHandler::startMoveEvent, this, &MyWindow::startMoveEvent);

    // connect(addressBar, &QLineEdit::returnPressed, this, &MyWindow::loadPage);
    // connect(backButton, &QPushButton::clicked, this, &MyWindow::goBack);
    // connect(forwardButton, &QPushButton::clicked, this, &MyWindow::goForward);
    // connect(reloadButton, &QPushButton::clicked, this, &MyWindow::reloadPage);
    connect(tabWidget, &QTabWidget::currentChanged, this, &MyWindow::updateAddressBar);
    connect(tabWidget->tabBar(), &QTabBar::tabCloseRequested, this, &MyWindow::closeTab);

    // Add a context menu to create new tabs
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &MyWindow::customContextMenuRequested, this, &MyWindow::showContextMenu);

    // Set layout in QWidget
    QWidget *window = new QWidget();
    // remove window borders
    window->setStyleSheet("QWidget { padding: 0px; margin: 0px; }");
    window->setLayout(mainLayout);

    // Set QWidget as the central layout of the main window
    this->setCentralWidget(window);
}
