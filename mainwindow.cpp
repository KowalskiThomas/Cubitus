#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "b2.h"
#include "config.h"
#include "filestreemodel.h"
#include "filesmodel.h"

MainWindow::MainWindow(B2* b2_, QWidget *parent)
    : b2(b2_),
      QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->filesTree->setModel(new FilesTreeModel(new FilesModel(b2, this), this));
}

MainWindow::~MainWindow()
{
    delete ui;
}

