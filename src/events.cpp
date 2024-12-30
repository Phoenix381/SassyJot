
#include "include/app_window.h"

// =============================================================================
// connecting corresponding app events
// =============================================================================
void AppWindow::registerEvents() {
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

    // setting up db api
    connect(handler, &ClickHandler::removeBookmarkEvent, this, &AppWindow::removeBookmark);
    connect(handler, &ClickHandler::addBookmarkEvent, db, &DBController::addBookmark);
    connect(handler, &ClickHandler::checkBookmarkEvent, this, &AppWindow::checkBookmark);

    // Add a context menu to create new tabs
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &AppWindow::customContextMenuRequested, this, &AppWindow::showContextMenu);
}