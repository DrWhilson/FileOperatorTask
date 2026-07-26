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

    QStringList selectedPatterns() const;
    QString selectedDirectory() const;

private slots:
    void browseDirectory();
    void addPattern();
    void removePattern();
    void clearAll();

private:
    void updateStartButton();

    QLineEdit *m_patternInput;
    QListWidget *m_patternList;
    QLabel *m_dirLabel;
    QPushButton *m_okBtn;

    QStringList m_patterns;
    QString m_directory;
};

#endif
