#include "settingsdialog.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(bool deleteAfterProcessing, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Настройки"));
    setMinimumWidth(350);

    auto *mainLayout = new QVBoxLayout(this);

    m_deleteCheck = new QCheckBox(tr("Удалять исходные файлы после обработки"));
    m_deleteCheck->setChecked(deleteAfterProcessing);
    mainLayout->addWidget(m_deleteCheck);

    mainLayout->addStretch();

    auto *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(tr("OK"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(tr("Отмена"), QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

SettingsDialog::~SettingsDialog() = default;

bool SettingsDialog::deleteAfterProcessing() const
{
    return m_deleteCheck->isChecked();
}
