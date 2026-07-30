#include "mainwindow.h"
#include "startupdialog.h"
#include "settingsdialog.h"
#include "fileprocessor.h"
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
#include <QSettings>
#include <QProgressBar>
#include <QThread>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_processor(nullptr)
    , m_workerThread(nullptr)
{
    ui->setupUi(this);

    QSettings s;
    m_outputPath = s.value("settings/outputPath", "").toString();
    m_onConflictMode = s.value("settings/onConflict", "overwrite").toString();
    m_runMode = s.value("settings/runMode", "once").toString();
    m_pollInterval = s.value("settings/pollInterval", 5).toInt();
    m_xorKey = s.value("settings/xorKey").toByteArray();
    m_deleteAfterProcessing = s.value("settings/deleteAfterProcessing", false).toBool();

    setWindowTitle(tr("FileOperatorTask"));
    resize(900, 600);

    // ── Тулбар ─────────────────────────────────────────
    m_toolbar = addToolBar(tr("Инструменты"));
    m_toolbar->setMovable(false);

    m_addAction = m_toolbar->addAction(tr("➕ Добавить файлы"));
    connect(m_addAction, &QAction::triggered, this, &MainWindow::showAddFilesDialog);

    m_removeAction = m_toolbar->addAction(tr("🗑 Удалить"));
    m_removeAction->setEnabled(false);
    connect(m_removeAction, &QAction::triggered, this, &MainWindow::removeSelectedFiles);

    m_toolbar->addSeparator();

    m_startAction = m_toolbar->addAction(tr("▶ Старт"));
    m_startAction->setEnabled(false);
    connect(m_startAction, &QAction::triggered, this, &MainWindow::startProcessing);

    m_pauseAction = m_toolbar->addAction(tr("⏸ Пауза"));
    m_pauseAction->setEnabled(false);
    connect(m_pauseAction, &QAction::triggered, this, &MainWindow::togglePause);

    m_toolbar->addSeparator();

    m_settingsAction = m_toolbar->addAction(tr("⚙ Настройки"));
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::showSettingsDialog);

    // ── Прогресс-бар ───────────────────────────────────
    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    statusBar()->addPermanentWidget(m_progressBar);

    // ── Центральный стек ───────────────────────────────
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_workerThread && m_workerThread->isRunning()) {
        auto ret = QMessageBox::question(this,
            tr("Подтверждение"),
            tr("Идёт обработка файлов. Завершить?"),
            QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
        if (m_processor)
            m_processor->cancel();
        m_workerThread->quit();
        m_workerThread->wait(5000);
    }
    event->accept();
}

// ── Страницы ─────────────────────────────────────────

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

    connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, [this]() {
            m_removeAction->setEnabled(!m_table->selectedItems().isEmpty());
        });

    m_stack->addWidget(page);
}

// ── Диалоги ──────────────────────────────────────────

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
    m_startAction->setEnabled(!m_filePaths.isEmpty());
}

void MainWindow::removeSelectedFiles()
{
    auto selectedRows = m_table->selectionModel()->selectedRows();
    std::sort(selectedRows.rbegin(), selectedRows.rend());
    for (const auto &index : selectedRows)
        m_filePaths.removeAt(index.row());

    if (m_filePaths.isEmpty()) {
        m_stack->setCurrentIndex(0);
        statusBar()->showMessage(tr("Файлы не выбраны"));
        m_removeAction->setEnabled(false);
        m_startAction->setEnabled(false);
    } else {
        populateFileList();
    }
}

void MainWindow::showSettingsDialog()
{
    SettingsDialog dlg(m_outputPath, m_onConflictMode, m_runMode,
                       m_pollInterval, m_xorKey, m_deleteAfterProcessing, this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    m_outputPath = dlg.outputPath();
    m_onConflictMode = dlg.onConflictMode();
    m_runMode = dlg.runMode();
    m_pollInterval = dlg.pollInterval();
    m_xorKey = dlg.xorKey();
    m_deleteAfterProcessing = dlg.deleteAfterProcessing();

    QSettings s;
    s.setValue("settings/outputPath", m_outputPath);
    s.setValue("settings/onConflict", m_onConflictMode);
    s.setValue("settings/runMode", m_runMode);
    s.setValue("settings/pollInterval", m_pollInterval);
    s.setValue("settings/xorKey", m_xorKey);
    s.setValue("settings/deleteAfterProcessing", m_deleteAfterProcessing);
}

// ── Обработка файлов ─────────────────────────────────

void MainWindow::startProcessing()
{
    if (m_filePaths.isEmpty())
        return;

    if (m_outputPath.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"),
            tr("Не указан путь для сохранения результатов.\n"
               "Задайте его в меню Настройки."));
        return;
    }

    QDir outDir(m_outputPath);
    if (!outDir.exists()) {
        QMessageBox::warning(this, tr("Ошибка"),
            tr("Указанная директория не существует:\n%1").arg(m_outputPath));
        return;
    }

    if (m_xorKey.size() != 8) {
        QMessageBox::warning(this, tr("Ошибка"),
            tr("XOR-ключ должен быть 8 байт (16 hex-символов).\n"
               "Задайте его в меню Настройки."));
        return;
    }

    setProcessingEnabled(false);

    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    statusBar()->showMessage(tr("Подготовка..."));

    m_workerThread = new QThread(this);
    m_processor = new FileProcessor(m_filePaths, m_outputPath, m_xorKey,
                                     m_onConflictMode, m_deleteAfterProcessing);
    m_processor->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, m_processor, &FileProcessor::process);
    connect(m_processor, &FileProcessor::fileProgress,
            this, &MainWindow::onFileProgress);
    connect(m_processor, &FileProcessor::fileCompleted,
            this, &MainWindow::onFileCompleted);
    connect(m_processor, &FileProcessor::allCompleted,
            this, &MainWindow::onAllCompleted);
    connect(m_processor, &FileProcessor::error,
            this, &MainWindow::onProcessingError);

    connect(m_processor, &FileProcessor::allCompleted,
            m_workerThread, &QThread::quit);
    connect(m_processor, &FileProcessor::allCompleted,
            m_processor, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished,
            m_workerThread, &QObject::deleteLater);

    m_pauseAction->setText(tr("⏸ Пауза"));
    m_pauseAction->setEnabled(true);

    m_workerThread->start();
}

void MainWindow::togglePause()
{
    if (!m_processor)
        return;

    if (m_pauseAction->text() == tr("⏸ Пауза")) {
        m_processor->pause();
        m_pauseAction->setText(tr("▶ Продолжить"));
        statusBar()->showMessage(tr("Обработка приостановлена"));
    } else {
        m_processor->resume();
        m_pauseAction->setText(tr("⏸ Пауза"));
        statusBar()->showMessage(tr("Обработка возобновлена"));
    }
}

// ── Слоты прогресса ──────────────────────────────────

void MainWindow::onFileProgress(const QString &fileName, qint64 current, qint64 total)
{
    if (total > 0) {
        int pct = static_cast<int>(current * 100 / total);
        m_progressBar->setValue(pct);
    }
    statusBar()->showMessage(tr("Обработка: %1 — %2 / %3")
        .arg(fileName, formatSize(current), formatSize(total)));
}

void MainWindow::onFileCompleted(const QString &fileName)
{
    statusBar()->showMessage(tr("Готово: %1").arg(fileName));
}

void MainWindow::onAllCompleted()
{
    m_progressBar->setVisible(false);
    m_progressBar->setValue(0);
    m_processor = nullptr;
    m_workerThread = nullptr;
    setProcessingEnabled(true);
    statusBar()->showMessage(tr("Обработка завершена"), 5000);
}

void MainWindow::onProcessingError(const QString &message)
{
    statusBar()->showMessage(message);
}

// ── Вспомогательное ──────────────────────────────────

void MainWindow::setProcessingEnabled(bool enabled)
{
    m_addAction->setEnabled(enabled);
    m_removeAction->setEnabled(enabled && !m_table->selectedItems().isEmpty());
    m_settingsAction->setEnabled(enabled);
    m_startAction->setEnabled(enabled && !m_filePaths.isEmpty());
    m_pauseAction->setEnabled(!enabled);
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
