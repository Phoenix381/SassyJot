
#include "include/controllers.h"

#include <QString>
#include <QWebEngineView>

// =============================================================================
// tabs controller
// =============================================================================

// constructor
TabController::TabController(AppWindow *window, QTabWidget *tabWidget, QWebEngineView *webControls, QWebEngineView *dev_view, DBController *db) {
	this->window = window;
	this->tabWidget = tabWidget;
	this->webControls = webControls;
	this->dev_view = dev_view;
	this->db = db;
}

// =============================================================================
// triggered from front
// =============================================================================

// creating new tab backend
void TabController::createTab(QString url) {
	// crating web view for tab
    QWebEngineView *webView = new QWebEngineView();

	// loading url if needed
    if (!url.isEmpty())
        webView->load(QUrl(url));
    else
        webView->load(QUrl("qrc:/html/dashboard.html"));

    // TODO memory usage check when removing tabs
    // setting up web controls for tab for dev view
    QWebChannel *channel = new QWebChannel(webView);
    channel->registerObject(QStringLiteral("app"), this);
    channel->registerObject(QStringLiteral("db"), db);
    webView->page()->setWebChannel(channel);

    // adding tab to tab widget
    int index = tabWidget->addTab(webView, "New Tab");
    std::cout << "Added tab " << index << std::endl;
    tabWidget->setCurrentIndex(index);

	// can`t remember why
    tabWidget->setTabsClosable(true);

    // connect dev view if needed
    if(dev_view) {
        webView->page()->setDevToolsPage(dev_view->page());
    }

    // updating tab titles on page event
    connect(webView->page(), &QWebEnginePage::titleChanged, this, [this, webView](const QString &title) {
        int index = tabWidget->indexOf(webView);
        if (index != -1) {
        	// sending to js
            webControls->page()->runJavaScript(std::format(
                "updateTabTitle({0},'{1}');", index, title.toStdString()
            ).c_str());
        }
    });

    // updating tab icons on page event
    connect(webView->page(), &QWebEnginePage::iconChanged, this, [this, webView](const QIcon &icon) {
        int index = tabWidget->indexOf(webView);
        if (index != -1) {
            // converting icon to pixmap (resizing and converting to base64 later)
            auto pixmap = icon.pixmap(16, 16);
            QByteArray pixels;
            QBuffer buffer(&pixels);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "PNG");

			// sending to js
            webControls->page()->runJavaScript(std::format(
                "updateTabIcon({0},'{1}');", index, pixels.toBase64().toStdString()
            ).c_str());
        }
    });

    // url change event
    connect( webView->page(), &QWebEnginePage::urlChanged, this, [this, webView](const QUrl &url) {
        int index = tabWidget->indexOf(webView);
        int currentIndex = tabWidget->currentIndex();

        // updating adress bar
        if (index == currentIndex)
            webControls->page()->runJavaScript(std::format(
                "updateTabURL('{0}');", url.toString().toStdString()
            ).c_str());

        // updating fav icon
        if (db->checkBookmark(url.toString()))
            webControls->page()->runJavaScript("setFavIcon(true);");
        else
            webControls->page()->runJavaScript("setFavIcon(false);");

        // saving to current workspace
        db->updateWorkspaceUrl(index, url.toString());
    }); 
}

// close tab on backend
void TabController::closeTab(int index) {
    // updating workspace
    db->removeWorkspaceUrl(index);

    // calculating new index
    int currentIndex = tabWidget->currentIndex();
    int nextIndex;

	if (index == tabWidget->count() - 1)
		nextIndex = index - 1;
	else
		nextIndex = index;

    // removing tab
    tabWidget->setCurrentIndex(nextIndex);
    tabWidget->removeTab(index);

    // switchig dev view
    if(dev_view) {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
        webView->page()->setDevToolsPage(dev_view->page());
    }
}

// close all tabs on backend
void TabController::closeAllTabs() {
	tabWidget->clear();
}

// select tab by index
void TabController::selectTab(int index) {
	tabWidget->setCurrentIndex(index);

	// switching dev view
    if(dev_view) {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
        webView->page()->setDevToolsPage(dev_view->page());
    }
}

// =============================================================================
// triggered from back, should update front manually
// =============================================================================

// closing currently selected tab
void TabController::closeCurrentTab() {
	// removing element on front
	webControls->page()->runJavaScript("closeCurrentTab();");

	closeTab(tabWidget->currentIndex());
}

// selecting next tab
void TabController::nextTab() {
	int currentIndex = tabWidget->currentIndex();
	int nextIndex;

	if (currentIndex == tabWidget->count() - 1)
		nextIndex = 0;
	else
		nextIndex = currentIndex + 1;

	switchTab(nextIndex);
}

// selecting previous tab
void TabController::prevTab() {
	int currentIndex = tabWidget->currentIndex();
	int nextIndex;

	if (currentIndex == 0)
		nextIndex = tabWidget->count() - 1;
	else
		nextIndex = currentIndex - 1;

	switchTab(nextIndex);
}

// selecting tab by index
void TabController::switchTab(int index){
	// switching tabWidget
	tabWidget->setCurrentIndex(index);

	// switching dev view
    if(dev_view) {
        QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
        webView->page()->setDevToolsPage(dev_view->page());
    }

    // update address bar
    auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url();
    std::cout << url.toString().toStdString() << std::endl;
    webControls->page()->runJavaScript(std::format(
        "updateTabURL('{0}');", url.toString().toStdString()
    ).c_str());
	
	// selecting new index on front
	webControls->page()->runJavaScript(std::format(
		"selectTab({0});", index
	).c_str());
}

// creating empty tab in gui
void TabController::requestEmptyTab() {
	requestNewTab("");
}

// creating new tab in gui
void TabController::requestNewTab(QString url) {
	// createTab(url);

    webControls->page()->runJavaScript(std::format(   
    	"newTab('{0}');", url.toStdString()
	).c_str());
}

// open all tabs from selected workspace by id
void TabController::loadWorkspaceTabs(int workspace_id) {
    auto urls = db->getWorkspaceUrls(workspace_id);
    std::cout << "Loading " << urls.size() << " tabs from workspace " << workspace_id << std::endl;

    for (auto el : urls)
        requestNewTab(QString::fromStdString(el.url));

    if (urls.size() == 0) {
    	std::cout << "No tabs in workspace " << workspace_id << std::endl;
		requestEmptyTab();
    }

    std::cout << "Finished loading tabs from workspace " << workspace_id << std::endl;
}




// go back in history
void TabController::pageBack() {
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
    if (webView) {
        webView->back();
    }
}

// go forward in history
void TabController::pageForward() {
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
    if (webView) {
        webView->forward();
    }
}

// reload current page
void TabController::pageReload() {
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());
    if (webView) {
        webView->reload();
    }
}

// change url
void TabController::pageChangeUrl(QString url) {
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());   
    if (webView) {
        // TODO validate url properly
        if (!url.startsWith("http://") && !url.startsWith("https://"))
            url.prepend("http://");

        webView->setUrl(QUrl(url));
    }
}