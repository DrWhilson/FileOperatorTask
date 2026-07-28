#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QCheckBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(bool deleteAfterProcessing, QWidget *parent = nullptr);
    ~SettingsDialog() override;

    bool deleteAfterProcessing() const;

private:
    QCheckBox *m_deleteCheck;
};

#endif
