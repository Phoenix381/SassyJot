
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
    // check resources
    // QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //     qDebug() << it.next();
    // }
    //     webView->load(QUrl("qrc:/index.html"));


    QApplication app(argc, argv);

    AppWindow window;
    window.show();

    return app.exec();
}
