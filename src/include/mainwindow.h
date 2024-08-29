#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include <QWidget>
#include <QMainWindow>

#include <QScreen>
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

#include <vector>
#include <format>

class AppWindow : public QMainWindow {
    Q_OBJECT
public:
    AppWindow();

private:
    // window controls
    QWebEngineView *webControls;
    // main page
    QTabWidget *tabWidget;

    // tabs
    std::vector<QWebEngineView *> tabs;
    int currentTab = 0;

    // dragging window
    bool dragging = false;
    bool maximized = false;
    QPoint localPos;
    QSize lastSize;

    // creating new tab
    // void createTab() {
        
    // }

protected:
    // handling dragging window
    void mouseMoveEvent(QMouseEvent *event) override {
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

    void createTab() {
        QWebEngineView *webView = new QWebEngineView();
        webView->setUrl(QUrl("https://www.google.com"));

        int index = tabWidget->addTab(webView, "New Tab");
        tabWidget->setCurrentIndex(index);

        // connect(webView, &QWebEngineView::urlChanged, this, &AppWindow::updateAddressBar);
        tabWidget->setTabsClosable(true);

        connect(webView->page(), &QWebEnginePage::titleChanged, this, [this, webView](const QString &title) {
            int index = tabWidget->indexOf(webView);
            if (index != -1) {
                // tabWidget->setTabText(index, title);

                webControls->page()->runJavaScript(std::format(
                    "updateTabTitle({0},'{1}');", index, title.toStdString()
                ).c_str());

                // qDebug() << "Updated tab title: " << title;
            }
        });

        connect(webView->page(), &QWebEnginePage::iconChanged, this, [this, webView](const QIcon &icon) {
            int index = tabWidget->indexOf(webView);
            if (index != -1) {
                // tabWidget->setTabIcon(index, icon);
                auto pixmap = icon.pixmap(16, 16);
                QByteArray pixels;
                QBuffer buffer(&pixels);
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer, "PNG");

                webControls->page()->runJavaScript(std::format(
                    "updateTabIcon({0},'{1}');", index, pixels.toBase64().toStdString()
                ).c_str());

                // qInfo() << std::format("Updated tab icon: {}", pixels.toBase64().toStdString());
            }
        });
    }

    void changeTab(int index) {
        tabWidget->setCurrentIndex(index);
    }

    // catching move event from js
    void startMoveEvent() {
        dragging = true;
        this->grabMouse();

        localPos = this->mapFromGlobal(QCursor::pos());
        lastSize = this->size();
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
        connect(newTabAction, &QAction::triggered, this, &AppWindow::createNewTab);

        contextMenu.exec(mapToGlobal(pos));
    }

    void createNewTab() {
        // QTabWidget *tabWidget = findChild<QTabWidget *>();
        // if (tabWidget) {
        //     createTab();
        // }
        // createTab();
    }
};


// handling js events
class ClickHandler : public QObject {
    Q_OBJECT

signals:
    void closeRequested();
    void maximizeRequested();
    void minimizeRequested();

    void createTabEvent();
    void tabChangeEvent(int index);

    void startMoveEvent();
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
        emit createTabEvent();
    }

    void requestChangeTab(int index) {
        emit tabChangeEvent(index);
    }

    void startMove() {
        emit startMoveEvent();
    }
};

#endif // MAINWINDOW_H
