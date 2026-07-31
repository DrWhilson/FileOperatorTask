#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QByteArray>

class QCheckBox;
class QLineEdit;
class QSpinBox;
class QRadioButton;
class QPushButton;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(const QString &outputPath, const QString &onConflictMode,
                   const QString &runMode, int pollInterval,
                   const QByteArray &xorKey, bool deleteAfterProcessing,
                   QWidget *parent = nullptr);
    ~SettingsDialog() override;

    QString outputPath() const;
    QString onConflictMode() const;
    QString runMode() const;
    int pollInterval() const;
    QByteArray xorKey() const;
    bool deleteAfterProcessing() const;

private slots:
    void browseOutputPath();
    void onRunModeChanged();

private:
    QLineEdit *m_outputPathEdit;
    QPushButton *m_browseBtn;
    QRadioButton *m_overwriteRadio;
    QRadioButton *m_counterRadio;
    QCheckBox *m_deleteCheck;
    QRadioButton *m_onceRadio;
    QRadioButton *m_timerRadio;
    QSpinBox *m_pollSpin;
    QLineEdit *m_xorKeyEdit;
};

#endif
