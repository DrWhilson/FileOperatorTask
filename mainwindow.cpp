#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(const QStringList &patterns, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_patterns(patterns)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
