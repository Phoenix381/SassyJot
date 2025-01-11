
#include "include/app_window.h"

// =============================================================================
// tab management functions
// =============================================================================

// creating new tab
void AppWindow::createTab(QString url) {
    QWebEngineView *webView = new QWebEngineView();

    if (!url.isEmpty())
        webView->load(QUrl(url));
    else
        webView->load(QUrl("qrc:/html/dashboard.html"));

    // TODO memory usage
    // setting up web controls for tab
    QWebChannel *channel = new QWebChannel(webView);
    channel->registerObject(QStringLiteral("app"), this);
    channel->registerObject(QStringLiteral("db"), db);
    webView->page()->setWebChannel(channel);

    // adding tab to tab widget
    int index = tabWidget->addTab(webView, "New Tab");
    tabWidget->setCurrentIndex(index);

    // connect(webView, &QWebEngineView::urlChanged, this, &AppWindow::updateAddressBar);
    tabWidget->setTabsClosable(true);

    // updating tab titles
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

    // updating tab icons
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

    // url change event
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

        // saving to current workspace
        db->updateWorkspaceUrl(index, url.toString());
    }); 
}

// creating new tab in gui
void AppWindow::requestNewTab() {
    webControls->page()->runJavaScript("newTab();");
}

// change tab by index
void AppWindow::changeTab(int index) {
    tabWidget->setCurrentIndex(index);
    QWebEngineView *webView = qobject_cast<QWebEngineView *>(tabWidget->currentWidget());

    webControls->page()->runJavaScript(std::format(
                "updateTabURL('{0}');", webView->url().toString().toStdString()
            ).c_str());
}

// close tab by index and set new index
void AppWindow::closeTab(int index, int nextIndex) {
    // updating workspace
    db->removeWorkspaceUrl(index);

    // closing tab
    tabWidget->removeTab(index);
    tabWidget->setCurrentIndex(nextIndex);
}

// close current tab
void AppWindow::closeCurrentTab() {
    webControls->page()->runJavaScript("closeCurrentTab();");

    // updating workspace
    db->removeWorkspaceUrl(tabWidget->currentIndex());
}

// close all open tabs
void AppWindow::closeAllTabs() {
    // TODO check for memory leaks
    tabWidget->clear();
}

// open all tabs from selected workspace
void AppWindow::loadWorkspaceTabs(int workspaceId) {
    auto urls = db->getWorkspaceUrls(workspaceId);
    std::cout << "Loading " << urls.size() << " tabs from workspace " << workspaceId << std::endl;

    for (auto el : urls) {
        webControls->page()->runJavaScript("newTab(false);");
        createTab(QString::fromStdString(el.url));
    }
}

// next tab
void AppWindow::nextTab() {
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

// prev tab
void AppWindow::prevTab() {
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
