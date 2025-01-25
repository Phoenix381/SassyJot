
#include <QApplication>
#include <QMainWindow>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QUrl>

#include <QShortcut>

#include <QDir>
#include <QDirIterator>

#include <iostream>
#include "include/app_window.h"

// =============================================================================
// message filtering
// =============================================================================

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    // Suppress specific warning messages
    if (msg.contains("of object 'AppWindow' has no notify signal")) {
        return;
    }

    // Default message handling
    switch (type) {
        case QtDebugMsg:
            qDebug() << msg;
            break;
        case QtWarningMsg:
            qWarning() << msg;
            break;
        case QtCriticalMsg:
            qCritical() << msg;
            break;
        case QtFatalMsg:
            qFatal("%s", qPrintable(msg));
            break;
    }
}

// =============================================================================
// app window initialization
// =============================================================================

AppWindow::AppWindow() {
    // message filter
    qInstallMessageHandler(customMessageHandler);

    // db api
    db = new DBController();
    
    // Set the window flags to remove the title bar
    setWindowFlags(Qt::FramelessWindowHint);

    // Set the window's size
    resize(800, 600);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // Controls widget
    webControls = new QWebEngineView();
    mainLayout->addWidget(webControls);
    webControls->load(QUrl("qrc:/html/controls.html"));
    // webControls->setStyleSheet("QWebEngineView { padding: 0px; margin: 0px; }");
    webControls->setFixedHeight(84);
    webControls->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // Tab widget
    tabWidget = new QTabWidget();
    mainLayout->addWidget(tabWidget);
    tabWidget->tabBar()->setVisible(false);

    // Developer tools
    dev_view = new QWebEngineView();
    mainLayout->addWidget(dev_view);
    webControls->page()->setDevToolsPage(dev_view->page());

    // setting up web controls for window
    QWebChannel *channel = new QWebChannel(webControls);
    channel->registerObject(QStringLiteral("app"), this);
    channel->registerObject(QStringLiteral("db"), db); // !!! ---
    webControls->page()->setWebChannel(channel);

    // creating controllers
    windowController = new WindowController(this, db);
    tabController = new TabController(this, tabWidget, webControls, dev_view, db);

    // registering controllers
    channel->registerObject(QStringLiteral("windowController"), windowController);
    channel->registerObject(QStringLiteral("tabController"), tabController);


    // init stuff
    registerEvents();
    registerHotkeys();

    // Set layout in QWidget
    QWidget *window = new QWidget();
    // remove window borders
    window->setStyleSheet("QWidget { padding: 0px; margin: 0px; }");
    window->setLayout(mainLayout);

    // Set QWidget as the central layout of the main window
    this->setCentralWidget(window);

    // Create initial tab with QWebEngineView
    connect(webControls, &QWebEngineView::loadFinished, this, 
        [this](){
            auto workspace_id = db->getSetting("workspace");
            
            // loading workspace tabs
            tabController->loadWorkspaceTabs(workspace_id.toInt());

            // // check if any tabs open
            // if (tabWidget->count() == 0)
            //     this->webControls->page()->runJavaScript("newTabElement.click();");
            
            // setting workspace color
            auto color = db->getCurrentWorkspaceColor().toStdString();
            auto func = QString("setWorkspaceColor('%1');").arg(color.c_str());
            this->webControls->page()->runJavaScript(func);

            // setting workspace id
            func = QString("setCurrentWorkspace('%1');").arg(workspace_id.toStdString().c_str());
            this->webControls->page()->runJavaScript(func);
        }
    );
}

// =============================================================================
// redefining mouse bahavior
// =============================================================================

// dragging window with mouse
void AppWindow::mouseMoveEvent(QMouseEvent *event) {
    if (windowController->dragging) {   
        QPoint globalPos = event->globalPosition().toPoint();
        window()->move(globalPos.x() - windowController->localPos.x(), globalPos.y() - windowController->localPos.y());

        // Find the screen where the cursor is located
        QScreen *screen = QGuiApplication::screenAt(globalPos);
        if (screen) {
            QRect screenGeometry = screen->geometry();

            if (!this->isMaximized() && globalPos.y() <= screenGeometry.top()) {
                windowController->maximized = true;
                this->showMaximized(); 
            } else if (globalPos.y() > screenGeometry.top()) {
                if(windowController->maximized) {
                    windowController->maximized = false;
                    this->showNormal();
                    this->resize(windowController->lastSize);
                }
            }
        }
    }
}

// handling lmb release
void AppWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (windowController->dragging && event->button() == Qt::LeftButton) {
        windowController->dragging = false;
        windowController->stopMove();
    }
}

// =============================================================================
// context menu
// =============================================================================

void AppWindow::showContextMenu(const QPoint &pos) {
    QMenu contextMenu;
    QAction *newTabAction = contextMenu.addAction("New Tab");
    connect(newTabAction, &QAction::triggered, tabController, &TabController::requestEmptyTab);

    contextMenu.exec(mapToGlobal(pos));
}

// =============================================================================
// hotkey actions
// =============================================================================

// address bar focus
// TODO pass webcontrols to controller
void AppWindow::focus() {
    if(! webControls->hasFocus()) {
        webControls->setFocus();
        // TODO more complex focus
    }
    webControls->page()->runJavaScript("gotFocus();");
}