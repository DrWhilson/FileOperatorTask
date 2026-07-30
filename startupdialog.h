#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QStringList>

class QLineEdit;
class QLabel;
class QPushButton;

// Диалог выбора файлов по маске.
// Пользователь указывает директорию и вводит маски/имена файлов,
// после чего диалог сканирует директорию и возвращает список найденных файлов.
class StartupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = nullptr);
    ~StartupDialog() override;

    // Возвращает абсолютные пути к файлам, отобранным по маскам
    QStringList selectedFiles() const;

    // Возвращает выбранную директорию
    QString selectedDirectory() const;

    // Возвращает список паттернов, введённых пользователем
    QStringList selectedPatterns() const;

    // Сканирование директории по переданным паттернам.
    // Паттерны делятся на маски (с '*') и точные имена файлов.
    static QStringList scanFiles(const QString &directory, const QStringList &patterns);

private slots:
    void browseDirectory();
    // Обрабатывает ввод маски: разбивает по запятым, запускает сканирование
    void addPattern();

private:
    QLineEdit *m_patternInput;    // поле ввода масок (например "*.txt, test.bin")
    QLabel *m_dirLabel;
    QPushButton *m_addBtn;        // кнопка "Добавить" — запускает поиск по маскам
    QStringList m_patterns;       // сохранённые паттерны (для переиспользования)
    QStringList m_files;          // результат: отобранные файлы
    QString m_directory;
};

#endif
