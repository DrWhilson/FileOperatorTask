#include "test_processor.h"
#include "../fileprocessor.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QThread>
#include <QTest>

namespace {

bool writeFile(const QString &path, const QByteArray &data)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    f.write(data);
    f.close();
    return true;
}

} // namespace

TestFileProcessor::TestFileProcessor()
    : m_tempDir(nullptr)
{
}

TestFileProcessor::~TestFileProcessor()
{
    delete m_tempDir;
}

void TestFileProcessor::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    m_inputDir = m_tempDir->path() + "/input";
    m_outputDir = m_tempDir->path() + "/output";
    QVERIFY(QDir().mkpath(m_inputDir));
    QVERIFY(QDir().mkpath(m_outputDir));

    m_xorKey = QByteArray::fromHex("0102030405060708");
}

void TestFileProcessor::cleanupTestCase()
{
    delete m_tempDir;
    m_tempDir = nullptr;
}

QString TestFileProcessor::runProcessor(const QStringList &files, const QString &conflictMode,
                                        bool deleteSource, const QString &outputDir)
{
    const QString outDir = outputDir.isEmpty() ? m_outputDir : outputDir;

    FileProcessor processor(files, outDir, m_xorKey, conflictMode, deleteSource);

    QString errorMsg;
    connect(&processor, &FileProcessor::error,
            [&errorMsg](const QString &message) { errorMsg = message; });

    processor.process();
    return errorMsg;
}

void TestFileProcessor::xorTransform()
{
    QString input = m_inputDir + "/xor.bin";
    QByteArray data(256, 0);
    for (int i = 0; i < 256; ++i)
        data[i] = static_cast<char>(i);
    QVERIFY(writeFile(input, data));

    QVERIFY2(runProcessor({input}, "overwrite", false).isEmpty(), "processing failed");

    QByteArray expected(256, 0);
    for (int i = 0; i < 256; ++i)
        expected[i] = static_cast<char>(
            static_cast<unsigned char>(i) ^ static_cast<unsigned char>(m_xorKey[i % 8]));

    QFile out(m_outputDir + "/xor.bin");
    QVERIFY(out.open(QIODevice::ReadOnly));
    QCOMPARE(out.readAll(), expected);
}

void TestFileProcessor::overwriteOutput()
{
    QString input = m_inputDir + "/overwrite.bin";
    QVERIFY(writeFile(input, QByteArray::fromHex("DEADBEEF")));

    QVERIFY2(runProcessor({input}, "overwrite", false).isEmpty(), "first run failed");
    QVERIFY(QFileInfo::exists(m_outputDir + "/overwrite.bin"));

    QVERIFY2(runProcessor({input}, "overwrite", false).isEmpty(), "second run failed");
    QVERIFY(QFileInfo::exists(m_outputDir + "/overwrite.bin"));
    QVERIFY(!QFileInfo::exists(m_outputDir + "/overwrite_1.bin"));
}

void TestFileProcessor::counterOutput()
{
    QString input = m_inputDir + "/counter.bin";
    QVERIFY(writeFile(input, QByteArray::fromHex("DEADBEEF")));

    QVERIFY2(runProcessor({input}, "counter", false).isEmpty(), "first run failed");
    QVERIFY(QFileInfo::exists(m_outputDir + "/counter.bin"));

    QVERIFY2(runProcessor({input}, "counter", false).isEmpty(), "second run failed");
    QVERIFY(QFileInfo::exists(m_outputDir + "/counter.bin"));
    QVERIFY(QFileInfo::exists(m_outputDir + "/counter_1.bin"));
}

void TestFileProcessor::cancelProcessing()
{
    QString input = m_inputDir + "/cancel.bin";
    QVERIFY(writeFile(input, QByteArray(16 * 1024 * 1024, 'A')));

    auto *thread = new QThread(this);
    auto *processor = new FileProcessor({input}, m_outputDir, m_xorKey, "overwrite", false);
    processor->moveToThread(thread);

    bool gotCancelled = false;
    connect(processor, &FileProcessor::cancelled, this,
            [&gotCancelled]() { gotCancelled = true; });
    connect(thread, &QThread::started, processor, &FileProcessor::process);
    connect(processor, &FileProcessor::cancelled, thread, &QThread::quit);

    thread->start();

    QTest::qWait(100);        // даём обработчику начать работу
    processor->pause();       // переводим в паузу (детерминированная точка)
    QTest::qWait(100);        // ждём, пока поток встанет на паузу
    processor->cancel();      // прерываем обработку

    QTRY_VERIFY_WITH_TIMEOUT(gotCancelled, 10000);
    thread->wait(5000);

    // Частичный файл должен быть удалён
    QVERIFY(!QFileInfo::exists(m_outputDir + "/cancel.bin"));

    delete processor;
    delete thread;
}

void TestFileProcessor::deleteSource()
{
    QString input = m_inputDir + "/delete.bin";
    QVERIFY(writeFile(input, QByteArray::fromHex("DEADBEEF")));

    QVERIFY2(runProcessor({input}, "overwrite", true).isEmpty(), "processing failed");

    QVERIFY(!QFileInfo::exists(input));
    QVERIFY(QFileInfo::exists(m_outputDir + "/delete.bin"));
}

void TestFileProcessor::nonexistentInput()
{
    QString err = runProcessor({m_inputDir + "/missing.bin"}, "overwrite", false);
    QVERIFY(!err.isEmpty());
}

void TestFileProcessor::emptyFile()
{
    QString input = m_inputDir + "/empty.bin";
    QVERIFY(writeFile(input, QByteArray()));

    QVERIFY2(runProcessor({input}, "overwrite", false).isEmpty(), "processing failed");

    QFile out(m_outputDir + "/empty.bin");
    QVERIFY(out.open(QIODevice::ReadOnly));
    QVERIFY(out.size() == 0);
}

void TestFileProcessor::largeFile()
{
    QString input = m_inputDir + "/large.bin";
    QByteArray data(200 * 1024, 0);
    for (int i = 0; i < data.size(); ++i)
        data[i] = static_cast<char>(i % 256);
    QVERIFY(writeFile(input, data));

    QVERIFY2(runProcessor({input}, "overwrite", false).isEmpty(), "processing failed");

    QByteArray expected(data.size(), 0);
    for (int i = 0; i < data.size(); ++i)
        expected[i] = static_cast<char>(
            static_cast<unsigned char>(data[i]) ^ static_cast<unsigned char>(m_xorKey[i % 8]));

    QFile out(m_outputDir + "/large.bin");
    QVERIFY(out.open(QIODevice::ReadOnly));
    QCOMPARE(out.readAll(), expected);
}

void TestFileProcessor::outputDirCreated()
{
    QString input = m_inputDir + "/mkdir.bin";
    QVERIFY(writeFile(input, QByteArray::fromHex("DEADBEEF")));

    const QString newDir = m_tempDir->path() + "/created_dir";
    QVERIFY(!QDir().exists(newDir));

    QVERIFY2(runProcessor({input}, "overwrite", false, newDir).isEmpty(), "processing failed");

    QVERIFY(QDir().exists(newDir));
    QVERIFY(QFileInfo::exists(newDir + "/mkdir.bin"));
}
