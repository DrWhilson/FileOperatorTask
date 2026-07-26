#include "mainwindow.h"
#include "startupdialog.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileInfo>
#include <QTableWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QToolBar>
#include <QAction>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("FileOperatorTask"));
    resize(900, 600);

    m_toolbar = addToolBar(tr("Инструменты"));
    m_toolbar->setMovable(false);
    auto *addAction = m_toolbar->addAction(tr("➕ Добавить файлы"));
    connect(addAction, &QAction::triggered, this, &MainWindow::showAddFilesDialog);

    m_stack = new QStackedWidget();
    setCentralWidget(m_stack);

    setupEmptyPage();
    setupTablePage();

    m_stack->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupEmptyPage()
{
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);

    layout->addStretch();

    auto *iconLabel = new QLabel(tr("📂"));
    iconLabel->setAlignment(Qt::AlignCenter);
    QFont iconFont = iconLabel->font();
    iconFont.setPointSize(48);
    iconLabel->setFont(iconFont);
    layout->addWidget(iconLabel);

    auto *textLabel = new QLabel(tr("Нет выбранных файлов"));
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setStyleSheet("color: gray; font-size: 16px;");
    layout->addWidget(textLabel);

    auto *btn = new QPushButton(tr("➕ Добавить файлы"));
    btn->setFixedSize(200, 40);
    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    layout->addStretch();

    connect(btn, &QPushButton::clicked, this, &MainWindow::showAddFilesDialog);

    m_stack->addWidget(page);
}

void MainWindow::setupTablePage()
{
    auto *page = new QWidget();
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);

    m_table = new QTableWidget(0, 3);
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

    layout->addWidget(m_table);

    m_stack->addWidget(page);
}

void MainWindow::showAddFilesDialog()
{
    StartupDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const QStringList newFiles = dlg.selectedFiles();
    if (newFiles.isEmpty())
        return;

    for (const QString &f : newFiles) {
        if (!m_filePaths.contains(f))
            m_filePaths.append(f);
    }

    populateFileList();
    m_stack->setCurrentIndex(1);
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
