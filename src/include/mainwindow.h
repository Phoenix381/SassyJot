#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QtGui>

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QLineEdit>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebEnginePage>
#include <QIcon>
#include <QTabBar>
#include <QMenu>

class MyWindow : public QMainWindow {
    Q_OBJECT
public:
    MyWindow();

private:
    QWebEngineView *webControls;
    QTabWidget *tabWidget;

    bool dragging = false;
    QPoint localPos;

    void createTab(QTabWidget *tabWidget) {
        QWebEngineView *webView = new QWebEngineView();
        webView->setUrl(QUrl("https://www.google.com"));

        int index = tabWidget->addTab(webView, "New Tab");
        tabWidget->setCurrentIndex(index);

        // connect(webView, &QWebEngineView::urlChanged, this, &MyWindow::updateAddressBar);
        tabWidget->setTabsClosable(true);

        connect(webView->page(), &QWebEnginePage::titleChanged, this, [this, webView, tabWidget](const QString &title) {
            int index = tabWidget->indexOf(webView);
            if (index != -1) {
                tabWidget->setTabText(index, title);
                qDebug() << "Updated tab title: " << title;
            }
        });
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        if (dragging) {   
            window()->move(event->globalPosition().toPoint().x() - localPos.x(), event->globalPosition().toPoint().y() - localPos.y());
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (dragging && event->button() == Qt::LeftButton) {
            dragging = false;
            this->releaseMouse();
        }
    }

public slots:
    void closeWindow() {
        close();   
    }

    void minimizeWindow() {
        showMinimized();
    }

    void toggleMaximizeRestore() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    }

    void createTabEvent() {
        
    }

    // catching move event from js
    void startMoveEvent(int x, int y) {
        dragging = true;
        this->grabMouse();

        localPos = this->mapFromGlobal(QCursor::pos());
    }

private slots:
    void loadPage() {
        QLineEdit *addressBar = findChild<QLineEdit *>();
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (addressBar && tabWidget) {
            QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
            if (webView) {
                webView->setUrl(QUrl::fromUserInput(addressBar->text()));
            }
        }
    }

    void updateAddressBar(int index) {
        QLineEdit *addressBar = findChild<QLineEdit *>();
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (addressBar && tabWidget) {
            QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->widget(index));
            if (webView) {
                addressBar->setText(webView->url().toString());
            }
        }
    }

    void goBack() {
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (tabWidget) {
            QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
            if (webView) {
                webView->back();
            }
        }
    }

    void goForward() {
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (tabWidget) {
            QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
            if (webView) {
                webView->forward();
            }
        }
    }

    void reloadPage() {
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (tabWidget) {
            QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
            if (webView) {
                webView->reload();
            }
        }
    }

    void closeTab(int index) {
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (tabWidget && tabWidget->count() > 1) {
            tabWidget->removeTab(index);
        }
    }

    void showContextMenu(const QPoint &pos) {
        QMenu contextMenu;
        QAction *newTabAction = contextMenu.addAction("New Tab");
        connect(newTabAction, &QAction::triggered, this, &MyWindow::createNewTab);

        contextMenu.exec(mapToGlobal(pos));
    }

    void createNewTab() {
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (tabWidget) {
            createTab(tabWidget);
        }
    }
};


// handling js events
class ClickHandler : public QObject {
    Q_OBJECT

signals:
    void closeRequested();
    void maximizeRequested();
    void minimizeRequested();

    void startMoveEvent(int x, int y);
public slots:
    void handleClick() {
        qDebug() << "Click event received!";
    }

    void requestClose() {
        emit closeRequested();
    }

    void requestMaximize() {
        emit maximizeRequested();
    }

    void requestMinimize() {
        emit minimizeRequested();
    }

    void requestNewTab() {
        
    }

    void startMove(int x, int y) {
        // qInfo() << "startMove: " << x << ", " << y;
        emit startMoveEvent(x, y);
    }
};

#endif // MYWINDOW_H
