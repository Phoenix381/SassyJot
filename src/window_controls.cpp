
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

// handling lmb press
void AppWindow::startMove() {
    dragging = true;
    this->grabMouse();

    localPos = this->mapFromGlobal(QCursor::pos());
    lastSize = this->size();
}

// handling lmb release
void AppWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (dragging && event->button() == Qt::LeftButton) {
        dragging = false;
        this->releaseMouse();
    }
}

// closing window
void AppWindow::closeWindow() {
    close();   
}

// minimizing window
void AppWindow::minimizeWindow() {
    showMinimized();
}

// maximizing window
void AppWindow::toggleMaximize() {
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

// =============================================================================
// current page handling
// =============================================================================

// go back in history
void AppWindow::pageBack() {
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
    if (webView) {
        webView->back();
    }
}

// go forward in history
void AppWindow::pageForward() {
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
    if (webView) {
        webView->forward();
    }
}

// reload current page
void AppWindow::pageReload() {
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
    if (webView) {
        webView->reload();
    }
}

// change url
void AppWindow::pageChangeUrl(QString url) {
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());   
    if (webView) {
        // TODO validate url properly
        if (!url.startsWith("http://") && !url.startsWith("https://"))
            url.prepend("http://");

        webView->setUrl(QUrl(url));
    }
}


// change url
void AppWindow::loadPage() {
    QLineEdit *addressBar = findChild<QLineEdit *>();
    QTabWidget *tabWidget = findChild<QTabWidget *>();
    if (addressBar && tabWidget) {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
        if (webView) {
            webView->setUrl(QUrl::fromUserInput(addressBar->text()));
        }
    }
}

// reload current page
void AppWindow::reloadPage() {
    QTabWidget *tabWidget = findChild<QTabWidget *>();
    if (tabWidget) {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
        if (webView) {
            webView->reload();
        }
    }
}

// 
void AppWindow::showContextMenu(const QPoint &pos) {
    QMenu contextMenu;
    QAction *newTabAction = contextMenu.addAction("New Tab");
    connect(newTabAction, &QAction::triggered, this, &AppWindow::createNewTab);

    contextMenu.exec(mapToGlobal(pos));
}

void AppWindow::createNewTab() {

}

void AppWindow::focus() {
    if(! webControls->hasFocus()) {
        // qDebug() << "Got focus";
        webControls->setFocus();
        // pass esc to webview
    }
    webControls->page()->runJavaScript("gotFocus();");
}