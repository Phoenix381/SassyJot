
#include "include/click_handler.h"

// =============================================================================
// window control
// =============================================================================
void ClickHandler::requestClose() {
    emit closeRequested();
}

void ClickHandler::requestMaximize() {
    emit maximizeRequested();
}

void ClickHandler::requestMinimize() {
    emit minimizeRequested();
}

// =============================================================================
// navigation
// =============================================================================
void ClickHandler::requestBack() {
    emit backRequested();   
}

void ClickHandler::requestForward() {
    emit forwardRequested();   
}

void ClickHandler::requestReload() {
    emit reloadRequested();
}

void ClickHandler::requestUrlChange(QString url) {
    emit urlChangeRequested(url);
}

// =============================================================================
// tab control
// =============================================================================
void ClickHandler::requestNewTab() {
    emit createTabEvent();
}

void ClickHandler::requestChangeTab(int index) {
    emit tabChangeEvent(index);
}

void ClickHandler::requestCloseTab(int index, int newIndex) {
    emit tabCloseEvent(index, newIndex);
}

void ClickHandler::startMove() {
    emit startMoveEvent();
}

// =============================================================================
// db interaction
// =============================================================================
void ClickHandler::addLink(QString url, QString title) {
    std::cout << "Adding link: " << url.toStdString() << " - " << title.toStdString() << std::endl;   
}

void ClickHandler::addBookmark(QString url, QString icon, QString title) {
    // qDebug() << "Adding bookmark: " << url << " - " << icon << " - " << title;
    emit addBookmarkEvent(url, icon, title);
}

void ClickHandler::removeBookmark() {
    emit removeBookmarkEvent();
}

void ClickHandler::checkBookmark() {
    // qDebug() << "Checking bookmark: " << url;
    emit checkBookmarkEvent();
}