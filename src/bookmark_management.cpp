
#include "include/app_window.h"

// =============================================================================
// bookmark management functions
// =============================================================================

// fav button click
void AppWindow::favClick() {
	auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url().toString();

	// check if url is valid
	if(! url.startsWith("http://") && ! url.startsWith("https://")) {
		return;
	}

	// check if url is already bookmarked
	if(! db->checkBookmark(url)) {
	    // save bookmark (QString url, QString title, QIcon icon) to (string url, string title, string icon)
	    std::cout << "Saving bookmark: " << url.toStdString() << std::endl;

	    auto currentIndex = tabWidget->currentIndex();
	    auto title = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->title();
	    auto icon = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->page()->icon();

	    auto pixmap = icon.pixmap(16, 16);
	    QByteArray pixels;
	    QBuffer buffer(&pixels);
	    buffer.open(QIODevice::WriteOnly);
	    pixmap.save(&buffer, "PNG");

	    db->addBookmark(url, pixels.toBase64(), title);
	}
}

// faving by hotkey
void AppWindow::favDialog() {
	favClick();

	webControls->setFocus();
	webControls->page()->runJavaScript("favDialog();");
}


// remove current page bookmark
void AppWindow::removeBookmark() {
	// remove bookmark (QString url)
	auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url().toString();
	db->removeBookmark(url);
	// webControls->page()->runJavaScript("removeBookmark();");
}

// checking current page bookmark
void AppWindow::checkBookmark() {
	auto url = qobject_cast<QWebEngineView *>(tabWidget->currentWidget())->url().toString();

	if (db->checkBookmark(url)) {
	    webControls->page()->runJavaScript("setFavIcon(true);");
	    qDebug() << "Found bookmark: " << url;
	} 
	else {
	    webControls->page()->runJavaScript("setFavIcon(false);");
	    qDebug() << "No bookmark found: " << url;
	}
}