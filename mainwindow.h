#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QStringList &filePaths, const QString &baseDir, QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void populateFileList();
    static QString formatSize(qint64 bytes);

    Ui::MainWindow *ui;
    QTableWidget *m_table;
    QStringList m_filePaths;
    QString m_baseDir;
};
#endif // MAINWINDOW_H
