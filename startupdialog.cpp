#include "startupdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>

StartupDialog::StartupDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Добавление файлов"));
    setMinimumWidth(500);

    auto *mainLayout = new QVBoxLayout(this);

    auto *dirLabel = new QLabel(tr("Директория для поиска файлов:"));
    mainLayout->addWidget(dirLabel);

    auto *dirLayout = new QHBoxLayout();
    m_dirLabel = new QLabel(tr("Директория не выбрана"));
    m_dirLabel->setStyleSheet("color: gray;");
    dirLayout->addWidget(m_dirLabel, 1);
    auto *browseBtn = new QPushButton(tr("Обзор…"));
    dirLayout->addWidget(browseBtn);
    mainLayout->addLayout(dirLayout);

    auto *separator = new QLabel();
    separator->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(separator);

    mainLayout->addWidget(new QLabel(tr("Маски или имена файлов:")));

    auto *inputLayout = new QHBoxLayout();
    m_patternInput = new QLineEdit();
    m_patternInput->setPlaceholderText("*.txt, *.bin, test.bin");
    inputLayout->addWidget(m_patternInput);
    auto *addBtn = new QPushButton(tr("Добавить"));
    inputLayout->addWidget(addBtn);
    mainLayout->addLayout(inputLayout);

    m_patternList = new QListWidget();
    m_patternList->setMaximumHeight(120);
    mainLayout->addWidget(m_patternList);

    auto *patternBtnLayout = new QHBoxLayout();
    auto *removeBtn = new QPushButton(tr("Удалить"));
    auto *clearAllBtn = new QPushButton(tr("Очистить всё"));
    patternBtnLayout->addWidget(removeBtn);
    patternBtnLayout->addWidget(clearAllBtn);
    mainLayout->addLayout(patternBtnLayout);

    auto *buttonBox = new QDialogButtonBox();
    m_okBtn = buttonBox->addButton(tr("OK"), QDialogButtonBox::AcceptRole);
    m_okBtn->setEnabled(false);
    buttonBox->addButton(tr("Отмена"), QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    connect(browseBtn, &QPushButton::clicked, this, &StartupDialog::browseDirectory);
    connect(addBtn, &QPushButton::clicked, this, &StartupDialog::addPattern);
    connect(removeBtn, &QPushButton::clicked, this, &StartupDialog::removePattern);
    connect(clearAllBtn, &QPushButton::clicked, this, &StartupDialog::clearAll);
    connect(m_patternInput, &QLineEdit::returnPressed, this, &StartupDialog::addPattern);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

StartupDialog::~StartupDialog() = default;

QStringList StartupDialog::selectedPatterns() const
{
    return m_patterns;
}

QString StartupDialog::selectedDirectory() const
{
    return m_directory;
}

void StartupDialog::updateStartButton()
{
    m_okBtn->setEnabled(!m_directory.isEmpty() && !m_patterns.isEmpty());
}

void StartupDialog::browseDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        tr("Выберите директорию с файлами"),
        m_directory.isEmpty() ? QString() : m_directory);
    if (dir.isEmpty())
        return;

    m_directory = dir;
    m_dirLabel->setText(dir);
    m_dirLabel->setStyleSheet("color: black;");
    updateStartButton();
}

void StartupDialog::addPattern()
{
    QString text = m_patternInput->text().trimmed();
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
        m_patternList->addItem(trimmed);
    }
    m_patternInput->clear();
    updateStartButton();
}

void StartupDialog::removePattern()
{
    auto items = m_patternList->selectedItems();
    for (auto *item : items) {
        m_patterns.removeAll(item->text());
        delete item;
    }
    updateStartButton();
}

void StartupDialog::clearAll()
{
    m_patterns.clear();
    m_patternList->clear();
    updateStartButton();
}
