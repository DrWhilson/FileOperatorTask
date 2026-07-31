#ifndef TEST_PROCESSOR_H
#define TEST_PROCESSOR_H

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QTemporaryDir>

class TestFileProcessor : public QObject
{
    Q_OBJECT

public:
    TestFileProcessor();
    ~TestFileProcessor() override;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void xorTransform();
    void overwriteOutput();
    void counterOutput();
    void cancelProcessing();
    void deleteSource();
    void nonexistentInput();
    void emptyFile();
    void largeFile();
    void outputDirCreated();

private:
    // Запускает FileProcessor во вспомогательном потоке и ждёт завершения.
    // Возвращает пустую строку при успехе или текст ошибки.
    QString runProcessor(const QStringList &files, const QString &conflictMode,
                         bool deleteSource, const QString &outputDir = QString());

    QTemporaryDir *m_tempDir;
    QString m_inputDir;
    QString m_outputDir;
    QByteArray m_xorKey;
};

#endif
