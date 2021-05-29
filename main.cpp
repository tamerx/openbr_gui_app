#include "mainwindow.h"
#include <QtWidgets>
#include <openbr/openbr_plugin.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    br::Context::initialize(argc, argv);

    MainWindow w;
    w.show();
    return app.exec();

}
