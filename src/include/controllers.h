
#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QMainWindow>


#include "db_api.h"
#include "app_window.h"

// forward declarations
class AppWindow;

// =============================================================================
// window controller
// =============================================================================

class WindowController : public QObject {
    Q_OBJECT
public:
    WindowController(AppWindow *window, DBController *db);

    // moving window
    bool dragging = false;
    QPoint localPos;
    // resizing window
    QSize lastSize;
    bool maximized = false;
private:
    AppWindow *window;
    DBController *db;
public slots:
    void startMove();
    void stopMove();
    void closeWindow();
    void minimizeWindow();
    void toggleMaximize();
};

// =============================================================================
// tab controller
// =============================================================================

class TabController : public QObject {
    Q_OBJECT
public:
    TabController(AppWindow *window, QTabWidget *tabWidget, QWebEngineView *webControls, QWebEngineView *dev_view, DBController *db);

    // tabs
    std::vector<QWebEngineView *> tabs;
    // int currentTab = 0;
private:
    AppWindow *window;
    QTabWidget *tabWidget;
    QWebEngineView *webControls;
    QWebEngineView *dev_view;
    DBController *db;
public slots:
    // triggered from front
    void createTab(QString url);
    void closeTab(int index);
    void closeAllTabs();
    void selectTab(int index);
    // TODO moving tabs on front

    // triggered from back, should update front manually
    void closeCurrentTab();
    void nextTab();
    void prevTab();
    void switchTab(int index);
    void requestEmptyTab();
    void requestNewTab(QString url);
    void loadWorkspaceTabs(int workspace_id);

    void pageBack();
    void pageForward();
    void pageReload();
    void pageChangeUrl(QString url);
};

// =============================================================================
// note controller
// =============================================================================

class NoteController : public QObject {
    Q_OBJECT
public:

private:

public slots:

};

#endif // CONTROLLERS_H