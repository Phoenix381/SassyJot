#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QUrl>

int main(int argc, char *argv[]) {
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
    layout->addWidget(webView);

    QWidget *centralWidget = new QWidget(&window);
    centralWidget->setLayout(layout);
    window.setCentralWidget(centralWidget);

    // Load initial URL
    webView->load(QUrl("http://www.example.com"));

    window.show();
    return app.exec();
}
