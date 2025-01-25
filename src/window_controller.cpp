
#include "include/controllers.h"

#include <QMainWindow>

// =============================================================================
// app window handling controller
// =============================================================================

// constructor
WindowController::WindowController(AppWindow *window, DBController *db) {
    this->window = window;
    this->db = db;
}

// handling lmb press from js controls
void WindowController::startMove() {
    dragging = true;
    window->grabMouse();

    localPos = window->mapFromGlobal(QCursor::pos());
    lastSize = window->size();
}

// stopping
void WindowController::stopMove() {
    window->releaseMouse();
}

// closing window
void WindowController::closeWindow() {
    window->close();   
}

// minimizing window
void WindowController::minimizeWindow() {
    window->showMinimized();
}

// maximizing window
void WindowController::toggleMaximize() {
    if (window->isMaximized()) {
        window->showNormal();
    } else {
        window->showMaximized();
    }
}
