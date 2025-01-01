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

    // handling tabs
    void createTab();
    void requestNewTab();
    void changeTab(int index);
    void closeTab(int index, int nextIndex);
    void closeCurrentTab();
    void nextTab();
    void prevTab();

    // handling bookmarks
    void favDialog();
    void removeBookmark();
    void checkBookmark();

    // focusing controls by pressing esc
    void focus();

    // catching move event from js
    void startMoveEvent();
    void goBack();
    void goForward();
    void reload();
    void changeUrl(QString url);

private slots:
    void loadPage();
    void reloadPage();
    void showContextMenu(const QPoint &pos);
    void createNewTab();
};

#endif // APP_WINDOW_H