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

private slots:
    void browseDirectory();
    // Обрабатывает ввод маски: разбивает по запятым, запускает сканирование
    void addPattern();

private:
    // Сканирует директорию, применяя список масок/имён файлов
    QStringList scanFiles(const QStringList &patterns);

    QLineEdit *m_patternInput;    // поле ввода масок (например "*.txt, test.bin")
    QLabel *m_dirLabel;
    QPushButton *m_addBtn;        // кнопка "Добавить" — запускает поиск по маскам
    QStringList m_files;          // результат: отобранные файлы
    QString m_directory;
};

#endif
