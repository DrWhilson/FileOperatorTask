#include "startupdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>

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
    m_addBtn = new QPushButton(tr("Добавить"));
    m_addBtn->setEnabled(false);
    inputLayout->addWidget(m_addBtn);
    mainLayout->addLayout(inputLayout);

    auto *cancelLayout = new QHBoxLayout();
    cancelLayout->addStretch();
    auto *cancelBtn = new QPushButton(tr("Отмена"));
    cancelLayout->addWidget(cancelBtn);
    mainLayout->addLayout(cancelLayout);

    connect(browseBtn, &QPushButton::clicked, this, &StartupDialog::browseDirectory);
    connect(m_addBtn, &QPushButton::clicked, this, &StartupDialog::addPattern);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_patternInput, &QLineEdit::returnPressed, this, &StartupDialog::addPattern);
    connect(m_patternInput, &QLineEdit::textChanged, this, [this]() {
        m_addBtn->setEnabled(!m_directory.isEmpty() && !m_patternInput->text().trimmed().isEmpty());
    });
}

StartupDialog::~StartupDialog() = default;

QStringList StartupDialog::selectedFiles() const
{
    return m_files;
}

QString StartupDialog::selectedDirectory() const
{
    return m_directory;
}

QStringList StartupDialog::selectedPatterns() const
{
    return m_patterns;
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
    m_addBtn->setEnabled(!m_patternInput->text().trimmed().isEmpty());
}

// Обработка введённой маски.
// Пользователь может ввести сразу несколько значений через запятую,
// например: "*.txt, *.bin, test.bin"
void StartupDialog::addPattern()
{
    if (m_directory.isEmpty())
        return;

    QString text = m_patternInput->text().trimmed();
    if (text.isEmpty())
        return;

    // Разделяем строку по запятым — получаем список отдельных масок/имён
    const QStringList parts = text.split(',', Qt::SkipEmptyParts);
    QStringList patterns;
    for (const QString &part : parts) {
        const QString trimmed = part.trimmed();
        if (!trimmed.isEmpty())
            patterns.append(trimmed);
    }

    if (patterns.isEmpty())
        return;

    // Сохраняем паттерны и директорию для повторного использования
    m_patterns = patterns;

    // Сканируем директорию и закрываем диалог, возвращая результат
    m_files = scanFiles(m_directory, patterns);
    accept();
}

// Сканирование директории по переданным паттернам.
// Паттерны делятся на два типа:
//   — маски (содержат '*'), например "*.txt"  — применяются через QDir::entryList
//   — конкретные имена файлов, например "test.bin" — проверяются через QDir::exists
// Результат: список абсолютных путей к найденным файлам (без дубликатов)
QStringList StartupDialog::scanFiles(const QString &directory, const QStringList &patterns)
{
    QDir dir(directory);
    if (!dir.exists())
        return {};

    // Разделяем паттерны на маски и конкретные имена
    QStringList masks;
    QStringList specific;
    for (const QString &p : patterns) {
        if (p.contains('*'))
            masks.append(p);
        else
            specific.append(p);
    }

    // Поиск по маскам (glob-шаблоны QDir)
    QStringList found;
    if (!masks.isEmpty())
        found = dir.entryList(masks, QDir::Files, QDir::Name);

    // Проверка конкретных имён файлов
    for (const QString &name : specific) {
        if (dir.exists(name) && !found.contains(name))
            found.append(name);
    }

    // Преобразуем имена в абсолютные пути
    QStringList result;
    for (const QString &name : found)
        result.append(dir.absoluteFilePath(name));

    return result;
}
