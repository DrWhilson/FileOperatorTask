#include "startupdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QDialogButtonBox>

StartupDialog::StartupDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("FileOperatorTask — Настройка входных файлов"));
    setMinimumWidth(450);

    auto *mainLayout = new QVBoxLayout(this);

    auto *label = new QLabel(tr("Введите расширения или имена файлов для обработки:"));
    mainLayout->addWidget(label);

    auto *inputLayout = new QHBoxLayout();
    m_input = new QLineEdit();
    m_input->setPlaceholderText("*.txt, *.bin, test.bin");
    inputLayout->addWidget(m_input);

    auto *addBtn = new QPushButton(tr("Добавить"));
    inputLayout->addWidget(addBtn);
    mainLayout->addLayout(inputLayout);

    m_list = new QListWidget();
    mainLayout->addWidget(m_list);

    auto *removeBtn = new QPushButton(tr("Удалить"));
    mainLayout->addWidget(removeBtn);

    auto *buttonBox = new QDialogButtonBox();
    auto *startBtn = buttonBox->addButton(tr("Начать обработку"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(tr("Отмена"), QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    connect(addBtn, &QPushButton::clicked, this, &StartupDialog::addPattern);
    connect(removeBtn, &QPushButton::clicked, this, &StartupDialog::removeSelected);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(m_input, &QLineEdit::returnPressed, this, &StartupDialog::addPattern);
}

StartupDialog::~StartupDialog() = default;

QStringList StartupDialog::selectedPatterns() const
{
    return m_patterns;
}

void StartupDialog::addPattern()
{
    const QString text = m_input->text().trimmed();
    if (text.isEmpty())
        return;

    const QStringList parts = text.split(',', Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        const QString trimmed = part.trimmed();
        if (trimmed.isEmpty())
            continue;
        if (m_patterns.contains(trimmed))
            continue;
        m_patterns.append(trimmed);
        m_list->addItem(trimmed);
    }
    m_input->clear();
}

void StartupDialog::removeSelected()
{
    auto selectedItems = m_list->selectedItems();
    for (auto *item : selectedItems) {
        m_patterns.removeAll(item->text());
        delete item;
    }
}
