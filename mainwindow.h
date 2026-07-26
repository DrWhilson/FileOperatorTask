#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QTableWidget;
class QStackedWidget;
class QPushButton;
class QToolBar;

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

private:
    void setupEmptyPage();
    void setupTablePage();
    void populateFileList();
    QStringList scanDirectory(const QString &dir, const QStringList &patterns);
    static QString formatSize(qint64 bytes);

    Ui::MainWindow *ui;
    QStackedWidget *m_stack;
    QTableWidget *m_table;
    QToolBar *m_toolbar;
    QStringList m_filePaths;
};
#endif // MAINWINDOW_H
