#ifndef WINDOW_CONTROLS_CPP
#define WINDOW_CONTROLS_CPP

#include "include/app_window.h"

// =============================================================================
// app window handling
// =============================================================================

// handling dragging window
void AppWindow::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {   
        QPoint globalPos = event->globalPosition().toPoint();
        window()->move(globalPos.x() - localPos.x(), globalPos.y() - localPos.y());

        // Find the screen where the cursor is located
        QScreen *screen = QGuiApplication::screenAt(globalPos);
        if (screen) {
            QRect screenGeometry = screen->geometry();

            if (!this->isMaximized() && globalPos.y() <= screenGeometry.top()) {
                maximized = true;
                this->showMaximized(); 
            } else if (globalPos.y() > screenGeometry.top()) {
                if(maximized) {
                    maximized = false;
                    this->showNormal();
                    this->resize(lastSize);
                }
            }
        }
    }
}

// handling lmb release
void AppWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (dragging && event->button() == Qt::LeftButton) {
        dragging = false;
        this->releaseMouse();
    }
}

// control buttons
void AppWindow::closeWindow() {
    close();   
}

void AppWindow::minimizeWindow() {
    showMinimized();
}

void AppWindow::toggleMaximizeRestore() {
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

#endif