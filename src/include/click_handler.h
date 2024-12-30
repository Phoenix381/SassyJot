#ifndef CLICK_HANDLER_H
#define CLICK_HANDLER_H

#include <QtGui>
#include <iostream>

// =============================================================================
// passes events from js to handlers
// =============================================================================
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
    void urlChangeRequested(QString url);

    // tabs
    void createTabEvent();
    void tabChangeEvent(int index);
    void tabCloseEvent(int index, int newIndex);

    // moving window
    void startMoveEvent();

    // db api
    void addLinkEvent(QString url);
    void addBookmarkEvent(QString url, QString icon, QString title);
    void removeBookmarkEvent();
    void checkBookmarkEvent();
public slots:
    //     qDebug() << "Click event received!";

    // window
    void requestClose();
    void requestMaximize();
    void requestMinimize();

    // navigation
    void requestBack();
    void requestForward();
    void requestReload();
    void requestUrlChange(QString url);

    // tabs
    void requestNewTab();
    void requestChangeTab(int index);
    void requestCloseTab(int index, int newIndex);
    void startMove();

    // db api
    void addLink(QString url, QString title);
    void addBookmark(QString url, QString icon, QString title);
    void removeBookmark();
    void checkBookmark();
};

#endif