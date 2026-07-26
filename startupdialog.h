#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QStringList>

class QLineEdit;
class QLabel;
class QPushButton;

class StartupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = nullptr);
    ~StartupDialog() override;

    QStringList selectedFiles() const;

private slots:
    void browseDirectory();
    void addPattern();

private:
    QStringList scanFiles(const QStringList &patterns);

    QLineEdit *m_patternInput;
    QLabel *m_dirLabel;
    QPushButton *m_addBtn;
    QStringList m_files;
    QString m_directory;
};

#endif
