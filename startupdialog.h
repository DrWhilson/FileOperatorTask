#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QStringList>

class QLineEdit;
class QListWidget;

class StartupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = nullptr);
    ~StartupDialog() override;

    QStringList selectedPatterns() const;

private slots:
    void addPattern();
    void removeSelected();

private:
    QLineEdit *m_input;
    QListWidget *m_list;
    QStringList m_patterns;
};

#endif
