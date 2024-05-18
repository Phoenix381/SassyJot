
#include <QApplication>
// #include <QMainWindow>
// #include <QWebEngineView>
// #include <QLineEdit>
// #include <QVBoxLayout>
// #include <QUrl>

// #include <QDir>
// #include <QDirIterator>

#include "include/mainwindow.h"

int main(int argc, char *argv[]) {
// QDirIterator it(":", QDirIterator::Subdirectories);
// while (it.hasNext()) {
//     qDebug() << it.next();
// }
//     webView->load(QUrl("qrc:/index.html"));

//     // debug
//     auto dev_view = new QWebEngineView();
//     layout->addWidget(dev_view);
//     webView->page()->setDevToolsPage(dev_view->page());

    QApplication app(argc, argv);

    MyWindow window;
    window.show();

    return app.exec();
}
