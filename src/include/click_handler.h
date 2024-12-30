#ifndef CLICK_HANDLER_H
#define CLICK_HANDLER_H

#include <QtGui>
#include <iostream>

// =============================================================================
// re-emits events from js to handlers
// =============================================================================
class ClickHandler : public QObject {
    Q_OBJECT

signals:
    // misc
    void startMoveEvent();

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

    // db api
    void addLinkEvent(QString url);
    void addBookmarkEvent(QString url, QString icon, QString title);
    void removeBookmarkEvent();
    void checkBookmarkEvent();
public slots:
    //     qDebug() << "Click event received!";
    void startMove(){ emit startMoveEvent(); };

    // window
    void requestClose(){ emit closeRequested(); };
    void requestMaximize(){ emit maximizeRequested(); };
    void requestMinimize(){ emit minimizeRequested(); };

    // navigation
    void requestBack(){ emit backRequested(); };
    void requestForward(){ emit forwardRequested(); };
    void requestReload(){ emit reloadRequested(); };
    void requestUrlChange(QString url){ emit urlChangeRequested(url); };

    // tabs
    void requestNewTab(){ emit createTabEvent(); };
    void requestChangeTab(int index){ emit tabChangeEvent(index); };
    void requestCloseTab(int index, int newIndex){ emit tabCloseEvent(index, newIndex); };

    // db api
    void addLink(QString url, QString title);
    void addBookmark(QString url, QString icon, QString title) { emit addBookmarkEvent(url, icon, title); };
    void removeBookmark() { emit removeBookmarkEvent(); };
    void checkBookmark() { emit checkBookmarkEvent(); };
};

#endif // CLICK_HANDLER_H