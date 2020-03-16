#include "mainwindow.h"

#include <QApplication>

#include "b2.h"

int main(int argc, char *argv[])
{
    qDebug() << "Cubitus starting";
    QApplication a(argc, argv);

    qDebug() << "Building API";
    auto b2 = new B2(&a);

    MainWindow w(b2);
    w.show();


    return a.exec();
}
