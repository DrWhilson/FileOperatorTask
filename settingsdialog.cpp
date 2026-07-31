#include "settingsdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QRegularExpressionValidator>

SettingsDialog::SettingsDialog(const QString &outputPath, const QString &onConflictMode,
                               const QString &runMode, int pollInterval,
                               const QByteArray &xorKey, bool deleteAfterProcessing,
                               QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Настройки"));
    setMinimumWidth(500);

    auto *mainLayout = new QVBoxLayout(this);

    // ── Выходные файлы ──────────────────────────────────
    auto *outputGroup = new QGroupBox(tr("Выходные файлы"));
    auto *outputLayout = new QVBoxLayout(outputGroup);

    outputLayout->addWidget(new QLabel(tr("Папка для результатов:")));

    auto *pathLayout = new QHBoxLayout();
    m_outputPathEdit = new QLineEdit();
    m_outputPathEdit->setText(outputPath);
    m_outputPathEdit->setPlaceholderText(tr("Выберите папку..."));
    pathLayout->addWidget(m_outputPathEdit);
    m_browseBtn = new QPushButton(tr("Обзор…"));
    pathLayout->addWidget(m_browseBtn);
    outputLayout->addLayout(pathLayout);

    outputLayout->addWidget(new QLabel(tr("При совпадении имени:")));
    auto *conflictLayout = new QHBoxLayout();
    m_overwriteRadio = new QRadioButton(tr("Перезаписывать"));
    m_counterRadio = new QRadioButton(tr("Добавлять счётчик"));
    conflictLayout->addWidget(m_overwriteRadio);
    conflictLayout->addWidget(m_counterRadio);
    outputLayout->addLayout(conflictLayout);

    if (onConflictMode == "counter")
        m_counterRadio->setChecked(true);
    else
        m_overwriteRadio->setChecked(true);

    m_deleteCheck = new QCheckBox(tr("Удалять исходные файлы после обработки"));
    m_deleteCheck->setChecked(deleteAfterProcessing);
    outputLayout->addWidget(m_deleteCheck);

    mainLayout->addWidget(outputGroup);

    // ── Режим работы ────────────────────────────────────
    auto *modeGroup = new QGroupBox(tr("Режим работы"));
    auto *modeLayout = new QVBoxLayout(modeGroup);

    auto *modeRadioLayout = new QHBoxLayout();
    m_onceRadio = new QRadioButton(tr("Однократно"));
    m_timerRadio = new QRadioButton(tr("По таймеру"));
    modeRadioLayout->addWidget(m_onceRadio);
    modeRadioLayout->addWidget(m_timerRadio);
    modeLayout->addLayout(modeRadioLayout);

    if (runMode == "timer")
        m_timerRadio->setChecked(true);
    else
        m_onceRadio->setChecked(true);

    auto *pollLayout = new QHBoxLayout();
    pollLayout->addWidget(new QLabel(tr("Интервал опроса:")));
    m_pollSpin = new QSpinBox();
    m_pollSpin->setRange(1, 3600);
    m_pollSpin->setValue(pollInterval);
    m_pollSpin->setSuffix(tr(" сек"));
    pollLayout->addWidget(m_pollSpin);
    pollLayout->addStretch();
    modeLayout->addLayout(pollLayout);

    mainLayout->addWidget(modeGroup);

    // ── Параметры XOR ───────────────────────────────────
    auto *xorGroup = new QGroupBox(tr("Параметры XOR"));
    auto *xorLayout = new QVBoxLayout(xorGroup);

    xorLayout->addWidget(new QLabel(tr("Ключ (16 hex-символов):")));

    m_xorKeyEdit = new QLineEdit();
    m_xorKeyEdit->setMaxLength(16);
    m_xorKeyEdit->setInputMask(QString()); // no input mask, use validator
    m_xorKeyEdit->setPlaceholderText("1234567890ABCDEF");
    QRegularExpression hexRegex("[0-9A-Fa-f]{0,16}");
    m_xorKeyEdit->setValidator(new QRegularExpressionValidator(hexRegex, this));

    QString hexStr;
    for (int i = 0; i < xorKey.size() && i < 8; ++i)
        hexStr += QString::number(static_cast<unsigned char>(xorKey[i]), 16).rightJustified(2, '0').toUpper();
    if (hexStr.isEmpty())
        hexStr = QString(16, '0');
    m_xorKeyEdit->setText(hexStr);

    xorLayout->addWidget(m_xorKeyEdit);
    mainLayout->addWidget(xorGroup);

    mainLayout->addStretch();

    // ── Кнопки ──────────────────────────────────────────
    auto *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(tr("OK"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(tr("Отмена"), QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    // ── Соединения ──────────────────────────────────────
    connect(m_browseBtn, &QPushButton::clicked, this, &SettingsDialog::browseOutputPath);
    connect(m_onceRadio, &QRadioButton::toggled, this, &SettingsDialog::onRunModeChanged);
    connect(m_timerRadio, &QRadioButton::toggled, this, &SettingsDialog::onRunModeChanged);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    onRunModeChanged();
}

SettingsDialog::~SettingsDialog() = default;

QString SettingsDialog::outputPath() const
{
    return m_outputPathEdit->text().trimmed();
}

QString SettingsDialog::onConflictMode() const
{
    return m_counterRadio->isChecked() ? "counter" : "overwrite";
}

QString SettingsDialog::runMode() const
{
    return m_timerRadio->isChecked() ? "timer" : "once";
}

int SettingsDialog::pollInterval() const
{
    return m_pollSpin->value();
}

QByteArray SettingsDialog::xorKey() const
{
    QString hex = m_xorKeyEdit->text().trimmed();
    hex = hex.rightJustified(16, '0');
    QByteArray key;
    for (int i = 0; i < 16; i += 2) {
        bool ok;
        key.append(static_cast<char>(hex.mid(i, 2).toUInt(&ok, 16)));
    }
    return key;
}

bool SettingsDialog::deleteAfterProcessing() const
{
    return m_deleteCheck->isChecked();
}

void SettingsDialog::browseOutputPath()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        tr("Выберите папку для результатов"),
        m_outputPathEdit->text().isEmpty() ? QString() : m_outputPathEdit->text());
    if (!dir.isEmpty())
        m_outputPathEdit->setText(dir);
}

void SettingsDialog::onRunModeChanged()
{
    m_pollSpin->setEnabled(m_timerRadio->isChecked());
}
