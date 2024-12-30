#ifndef APPWINDOW_H
#define APPWINDOW_H

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
#include <iostream>

#include "db_api.h"
#include "click_handler.h"

// =============================================================================
// main app window
// =============================================================================
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

    DBController *db;

    // js events handler
    ClickHandler *handler;

    // misc functions
    void registerEvents();
    void registerHotkeys();

protected:
    // handling window drag anad drop
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    // button events
    void closeWindow();
    void minimizeWindow();
    void toggleMaximizeRestore();

    // hanadling tabs
    void createTab() {
        QWebEngineView *webView = new QWebEngineView();
        webView->load(QUrl("qrc:/html/dashboard.html"));

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
            // check if faved
            if (db->checkBookmark(url.toString()))
                webControls->page()->runJavaScript("setFavIcon(true);");
            else
                webControls->page()->runJavaScript("setFavIcon(false);");
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

        // update address bar
        auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url();
        webControls->page()->runJavaScript(std::format(
            "updateTabURL('{0}');", url.toString().toStdString()
        ).c_str());

        // check if faved
        this->checkBookmark();
    }

    void prevTab() {
        auto currentIndex = tabWidget->currentIndex();

        if (currentIndex - 1 >= 0)
            tabWidget->setCurrentIndex(currentIndex - 1);
        else
            tabWidget->setCurrentIndex(tabWidget->count() - 1);

        webControls->page()->runJavaScript("prevTab();");

        // update address bar
        auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url();
        webControls->page()->runJavaScript(std::format(
            "updateTabURL('{0}');", url.toString().toStdString()
        ).c_str());

        // check if faved
        this->checkBookmark();
    }

    void favDialog() {
        // check if url is already bookmarked
        auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url().toString();
        if(! db->checkBookmark(url)) {
            // save bookmark (QString url, QString title, QIcon icon) to (string url, string title, string icon)
            auto currentIndex = tabWidget->currentIndex();
            auto title = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->title();
            auto icon = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->page()->icon();

            auto pixmap = icon.pixmap(16, 16);
            QByteArray pixels;
            QBuffer buffer(&pixels);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "PNG");

            // qDebug() << "Added bookmark: " << url << ", " << title << ", " << pixels.toBase64().toStdString();

            // Bookmark bookmark(0, url, title, pixels.toBase64().toStdString());
            db->addBookmark(url, pixels.toBase64(), title);
        } // else {
        //     qDebug() << "Bookmark already exists: " << url;
        // }

        webControls->setFocus();
        webControls->page()->runJavaScript("favDialog();");
    }

    void focus() {
        if(! webControls->hasFocus()) {
            // qDebug() << "Got focus";
            webControls->setFocus();
            // pass esc to webview
        }
        webControls->page()->runJavaScript("gotFocus();");
    }

    void removeBookmark() {
        // remove bookmark (QString url)
        auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url().toString();
        db->removeBookmark(url);
        // webControls->page()->runJavaScript("removeBookmark();");
    }

    void checkBookmark() {
        auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url().toString();

        if (db->checkBookmark(url)) {
            webControls->page()->runJavaScript("setFavIcon(true);");
            qDebug() << "Found bookmark: " << url;
        } 
        else {
            webControls->page()->runJavaScript("setFavIcon(false);");
            qDebug() << "No bookmark found: " << url;
        }
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

    void changeUrl(QString url) {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());   
        if (webView) {
            // TODO validate url properly
            if (!url.startsWith("http://") && !url.startsWith("https://"))
                url.prepend("http://");

            webView->setUrl(QUrl(url));
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



    void reloadPage() {
        QTabWidget *tabWidget = findChild<QTabWidget *>();
        if (tabWidget) {
            QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
            if (webView) {
                webView->reload();
            }
        }
    }


    void showContextMenu(const QPoint &pos) {
        QMenu contextMenu;
        QAction *newTabAction = contextMenu.addAction("New Tab");
        connect(newTabAction, &QAction::triggered, this, &AppWindow::createNewTab);

        contextMenu.exec(mapToGlobal(pos));
    }

    void createNewTab() {

    }
};

#endif // APP_WINDOW_H