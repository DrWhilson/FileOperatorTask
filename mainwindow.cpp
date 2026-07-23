#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(const QStringList &patterns, const QString &directory, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_patterns(patterns)
    , m_directory(directory)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
