#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QTableWidget;
class QStackedWidget;
class QPushButton;
class QToolBar;
class QAction;

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

private slots:
    void showAddFilesDialog();
    void removeSelectedFiles();
    void showSettingsDialog();

private:
    void setupEmptyPage();
    void setupTablePage();
    void populateFileList();
    static QString formatSize(qint64 bytes);

    Ui::MainWindow *ui;
    QStackedWidget *m_stack;
    QTableWidget *m_table;
    QToolBar *m_toolbar;
    QAction *m_removeAction;
    QAction *m_settingsAction;
    bool m_deleteAfterProcessing;
    QStringList m_filePaths;
};
#endif // MAINWINDOW_H
