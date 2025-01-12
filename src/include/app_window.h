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

#include <QWebEngineView>
#include <QWebChannel>

#include <vector>
#include <format>
#include <iostream>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "db_api.h"

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

    // misc functions
    void registerEvents();
    void registerHotkeys();

protected:
    // handling window drag anad drop
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void requestNewTab();
    void closeCurrentTab();
    void closeAllTabs();
    void loadWorkspaceTabs(int workspaceId);
    void nextTab();
    void prevTab();

    // handling bookmarks
    void favDialog();
    void favClick();

    // focusing controls by pressing esc
    void focus();

    // window controls
    void closeWindow();
    void toggleMaximize();
    void minimizeWindow();

    void startMove();

    // navigation
    void pageBack();
    void pageForward();
    void pageReload();
    void pageChangeUrl(QString url);

    // tabs
    void createTab(QString url);
    void changeTab(int index);
    void closeTab(int index, int newIndex);

    // db api
    void removeBookmark();
    void checkBookmark();
    
    // note api
    QString getNodes();
    QString getLinks();
    void addNote(QString title, QString content, int group_id = 1);

private slots:
    void loadPage();
    void reloadPage();
    void showContextMenu(const QPoint &pos);
    void createNewTab();
};

#endif // APP_WINDOW_H