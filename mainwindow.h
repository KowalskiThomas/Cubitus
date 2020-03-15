#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class B2;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    B2* b2;

public:
    MainWindow(B2*, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
