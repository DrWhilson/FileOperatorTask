#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QByteArray>

class QTableWidget;
class QStackedWidget;
class QPushButton;
class QToolBar;
class QAction;
class QProgressBar;
class QThread;
class QTimer;
class FileProcessor;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    static QString formatSize(qint64 bytes);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void showAddFilesDialog();
    void removeSelectedFiles();
    void showSettingsDialog();
    void startProcessing();
    void togglePause();
    void onPollTimer();

    void onFileProgress(const QString &fileName, qint64 current, qint64 total);
    void onFileCompleted(const QString &fileName);
    void onAllCompleted();
    void onProcessingError(const QString &message);

private:
    void setupEmptyPage();
    void setupTablePage();
    void populateFileList();
    void setProcessingEnabled(bool enabled);
    void runProcessor();

    Ui::MainWindow *ui;
    QStackedWidget *m_stack;
    QTableWidget *m_table;
    QToolBar *m_toolbar;
    QAction *m_addAction;
    QAction *m_removeAction;
    QAction *m_settingsAction;
    QAction *m_startAction;
    QAction *m_pauseAction;
    QProgressBar *m_progressBar;
    QTimer *m_pollTimer;
    QStringList m_filePaths;

    FileProcessor *m_processor;
    QThread *m_workerThread;

    // Данные для таймерного режима
    QString m_lastInputDir;
    QStringList m_lastPatterns;

    // Настройки (сохраняются в QSettings)
    QString m_outputPath;
    QString m_onConflictMode;
    QString m_runMode;
    int m_pollInterval;
    QByteArray m_xorKey;
    bool m_deleteAfterProcessing;
};

#endif
