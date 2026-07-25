#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileInfo>

MainWindow::MainWindow(const QStringList &filePaths, const QString &baseDir, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_filePaths(filePaths)
    , m_baseDir(baseDir)
{
    ui->setupUi(this);

    setWindowTitle(tr("FileOperatorTask — %1").arg(m_baseDir));
    resize(900, 600);

    auto *centralLayout = new QVBoxLayout(ui->centralwidget);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({tr("Файл"), tr("Размер"), tr("Дата изменения")});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);

    auto *header = m_table->horizontalHeader();
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    centralLayout->addWidget(m_table);

    populateFileList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void MainWindow::populateFileList()
{
    m_table->setRowCount(m_filePaths.size());
    for (int i = 0; i < m_filePaths.size(); ++i) {
        QFileInfo fi(m_filePaths[i]);

        auto *pathItem = new QTableWidgetItem(fi.absoluteFilePath());
        pathItem->setToolTip(fi.absoluteFilePath());
        m_table->setItem(i, 0, pathItem);

        auto *sizeItem = new QTableWidgetItem(formatSize(fi.size()));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_table->setItem(i, 1, sizeItem);

        auto *dateItem = new QTableWidgetItem(
            fi.lastModified().toString("yyyy-MM-dd HH:mm"));
        dateItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 2, dateItem);
    }

    statusBar()->showMessage(tr("Выбрано файлов: %1").arg(m_filePaths.size()));
}

QString MainWindow::formatSize(qint64 bytes)
{
    if (bytes < 1024)
        return QString::number(bytes) + " Б";
    if (bytes < 1024 * 1024)
        return QString::number(bytes / 1024.0, 'f', 1) + " КБ";
    if (bytes < 1024LL * 1024 * 1024)
        return QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + " МБ";
    return QString::number(bytes / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " ГБ";
}
