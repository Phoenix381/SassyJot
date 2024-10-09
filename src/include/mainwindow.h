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

        connect( webView->page(), &QWebEnginePage::urlChanged, this, [this, webView](const QUrl &url) {
            int index = tabWidget->indexOf(webView);
            int currentIndex = tabWidget->currentIndex();
            if (index == currentIndex)
                webControls->page()->runJavaScript(std::format(
                    "updateTabURL('{0}');", url.toString().toStdString()
                ).c_str());
        });   
    }

    void requestNewTab() {
        webControls->page()->runJavaScript("newTab();");
    }

    void changeTab(int index) {
        tabWidget->setCurrentIndex(index);
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());

        webControls->page()->runJavaScript(std::format(
                    "updateTabURL('{0}');", webView->url().toString().toStdString()
                ).c_str());
    }

    void closeTab(int index, int newIndex) {
        tabWidget->removeTab(index);
        tabWidget->setCurrentIndex(newIndex);
    }

    void closeCurrentTab() {
        webControls->page()->runJavaScript("closeCurrentTab();");
    }

    void nextTab() {
        auto currentIndex = tabWidget->currentIndex();

        if (currentIndex + 1 < tabWidget->count())
            tabWidget->setCurrentIndex(currentIndex + 1);
        else
            tabWidget->setCurrentIndex(0);

        webControls->page()->runJavaScript("nextTab();");
    }

    void prevTab() {
        auto currentIndex = tabWidget->currentIndex();

        if (currentIndex - 1 >= 0)
            tabWidget->setCurrentIndex(currentIndex - 1);
        else
            tabWidget->setCurrentIndex(tabWidget->count() - 1);

        webControls->page()->runJavaScript("prevTab();");
    }

    // catching move event from js
    void startMoveEvent() {
        dragging = true;
        this->grabMouse();

        localPos = this->mapFromGlobal(QCursor::pos());
        lastSize = this->size();
    }

    void goBack() {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
        if (webView) {
            webView->back();
        }
    }

    void goForward() {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
        if (webView) {
            webView->forward();
        }
    }

    void reload() {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
        if (webView) {
            webView->reload();
        }
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

    // void updateAddressBar(int index) {
    //     QLineEdit *addressBar = findChild<QLineEdit *>();
    //     QTabWidget *tabWidget = findChild<QTabWidget *>();
    //     if (addressBar && tabWidget) {
    //         QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->widget(index));
    //         if (webView) {
    //             addressBar->setText(webView->url().toString());
    //         }
    //     }
    // }


    void reloadPage() {
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (tabWidget) {
            QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
            if (webView) {
                webView->reload();
            }
        }
    }

    // void closeTab(int index) {
    //     QTabWidget *tabWidget = findChild<QTabWidget *>();
    //     if (tabWidget && tabWidget->count() > 1) {
    //         tabWidget->removeTab(index);
    //     }
    // }

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
    // window
    void closeRequested();
    void maximizeRequested();
    void minimizeRequested();

    // navigation
    void backRequested();
    void forwardRequested();
    void reloadRequested();

    // tabs
    void createTabEvent();
    void tabChangeEvent(int index);
    void tabCloseEvent(int index, int newIndex);

    void startMoveEvent();
public slots:
    // void handleClick() {
    //     qDebug() << "Click event received!";
    // }

    // window
    void requestClose() {
        emit closeRequested();
    }

    void requestMaximize() {
        emit maximizeRequested();
    }

    void requestMinimize() {
        emit minimizeRequested();
    }

    // navigation
    void requestBack() {
        emit backRequested();   
    }

    void requestForward() {
        emit forwardRequested();   
    }

    void requestReload() {
        emit reloadRequested();
    }

    // tabs
    void requestNewTab() {
        emit createTabEvent();
    }

    void requestChangeTab(int index) {
        emit tabChangeEvent(index);
    }

    void requestCloseTab(int index, int newIndex) {
        emit tabCloseEvent(index, newIndex);
    }

    void startMove() {
        emit startMoveEvent();
    }
};

#endif // MAINWINDOW_H
