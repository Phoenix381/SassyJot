
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
#include "controllers.h"

// forward declaration
class WindowController;
class TabController;

// =============================================================================
// main app window
// =============================================================================
class AppWindow : public QMainWindow {
    Q_OBJECT
public:
    AppWindow();

private:
    // main gui elements
    QWebEngineView *webControls;
    QWebEngineView *dev_view;
    QTabWidget *tabWidget;

    // controllers
    WindowController *windowController;
    TabController *tabController;

    DBController *db;

    // misc functions
    void registerEvents();
    void registerHotkeys();

protected:
    // handling window drag anad drop
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    // focusing controls by pressing esc
    // TODO move to controller
    void focus();

    // handling bookmarks
    void favDialog();
    void favClick();

    // db api
    void removeBookmark();
    void checkBookmark();
    
    // note api
    QString getNodes();
    QString getLinks();
    void addNote(QString title, QString content, int group_id = 1);
    void updateNote(int id, QString title, QString content, int group_id = 1);

private slots:
    void showContextMenu(const QPoint &pos);
};

#endif // APP_WINDOW_H