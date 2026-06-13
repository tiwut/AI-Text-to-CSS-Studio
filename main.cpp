#include <QApplication>
#include "mainwindow.hpp"

int main(int argc, char *argv[]) {
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
    );

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
