#include "mainwindow.h"

#include <QApplication>

#include "b2.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto b2 = new B2(&a);

    MainWindow w(b2);
    w.show();


    return a.exec();
}
