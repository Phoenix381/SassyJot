
#include "include/app_window.h"

// =============================================================================
// tab management functions
// =============================================================================

// creating new tab
void AppWindow::createTab() {
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
    tabWidget->removeTab(index);
    tabWidget->setCurrentIndex(nextIndex);
}

// close current tab
void AppWindow::closeCurrentTab() {
    webControls->page()->runJavaScript("closeCurrentTab();");
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
