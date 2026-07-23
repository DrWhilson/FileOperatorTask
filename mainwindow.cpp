#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QLabel>

MainWindow::MainWindow(const QStringList &patterns, const QString &directory, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_patterns(patterns)
    , m_directory(directory)
{
    ui->setupUi(this);

    setWindowTitle(tr("FileOperatorTask — %1").arg(m_directory));
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
    QStringList specificFiles;
    QStringList masks;

    for (const QString &p : m_patterns) {
        if (p.contains('*'))
            masks.append(p);
        else
            specificFiles.append(p);
    }

    QDir dir(m_directory);
    if (!dir.exists())
        return;

    QStringList fileNames;
    if (!masks.isEmpty())
        fileNames = dir.entryList(masks, QDir::Files, QDir::Name);

    for (const QString &name : specificFiles) {
        if (dir.exists(name)) {
            if (!fileNames.contains(name))
                fileNames.append(name);
        }
    }

    m_table->setRowCount(fileNames.size());
    for (int i = 0; i < fileNames.size(); ++i) {
        QFileInfo fi(dir.absoluteFilePath(fileNames[i]));

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

    statusBar()->showMessage(tr("Найдено файлов: %1").arg(fileNames.size()));
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
