
#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QUrl>

#include <QDir>
#include <QDirIterator>

int main(int argc, char *argv[]) {
    // QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //     qDebug() << it.next();
    // }

    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Web Browser");

    // WebView
    QWebEngineView *webView = new QWebEngineView(&window);

    // Address bar
    QLineEdit *addressBar = new QLineEdit(&window);
    addressBar->setPlaceholderText("Enter URL");
    QObject::connect(addressBar, &QLineEdit::returnPressed, [&]() {
        QString url = addressBar->text();
        if (!url.startsWith("http://") && !url.startsWith("https://")) {
            url.prepend("http://");
        }
        webView->load(QUrl(url));
    });

    // Layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(addressBar);
    layout->addSpacing(10); // Add spacing between address bar and web view
    layout->addWidget(webView);

    QWidget *centralWidget = new QWidget(&window);
    centralWidget->setLayout(layout);
    window.setCentralWidget(centralWidget);

    // Load initial URL (from react frontend)
    webView->load(QUrl("qrc:/index.html"));

    // debug
    auto dev_view = new QWebEngineView();
    layout->addWidget(dev_view);
    webView->page()->setDevToolsPage(dev_view->page());

    // Make the window fullscreen but windowed
    window.showMaximized();

    return app.exec();
}
