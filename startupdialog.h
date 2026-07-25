#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QStringList>

class QLineEdit;
class QListWidget;
class QLabel;
class QPushButton;

class StartupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = nullptr);
    ~StartupDialog() override;

    QStringList selectedFiles() const;
    QString selectedDirectory() const;

private slots:
    void browseDirectory();
    void addPattern();
    void removePattern();
    void clearAll();

private:
    void scanFiles();
    void updateStartButton();

    QLineEdit *m_patternInput;
    QListWidget *m_patternList;
    QListWidget *m_fileList;
    QLabel *m_dirLabel;
    QPushButton *m_startBtn;

    QStringList m_patterns;
    QStringList m_files;
    QString m_directory;
};

#endif
