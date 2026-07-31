#include "test_scanfiles.h"
#include "../startupdialog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QTest>

TestScanFiles::TestScanFiles()
    : m_tempDir(nullptr)
{
}

TestScanFiles::~TestScanFiles()
{
    delete m_tempDir;
}

void TestScanFiles::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    QDir dir(m_tempDir->path());
    auto touch = [&](const QString &name) {
        QFile f(dir.absoluteFilePath(name));
        if (!f.open(QIODevice::WriteOnly))
            qWarning("Failed to create test file: %s", qPrintable(name));
        f.close();
    };

    touch("file1.txt");
    touch("file2.txt");
    touch("test.bin");
    touch("data.bin");
    touch("notes.md");
    touch("readme");
}

void TestScanFiles::cleanupTestCase()
{
    delete m_tempDir;
    m_tempDir = nullptr;
}

void TestScanFiles::globTxt()
{
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {"*.txt"});
    QCOMPARE(result.size(), 2);

    QSet<QString> names;
    for (const QString &path : result)
        names.insert(QFileInfo(path).fileName());
    QVERIFY(names.contains("file1.txt"));
    QVERIFY(names.contains("file2.txt"));
}

void TestScanFiles::globBin()
{
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {"*.bin"});
    QCOMPARE(result.size(), 2);

    QSet<QString> names;
    for (const QString &path : result)
        names.insert(QFileInfo(path).fileName());
    QVERIFY(names.contains("test.bin"));
    QVERIFY(names.contains("data.bin"));
}

void TestScanFiles::specificFile()
{
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {"test.bin"});
    QCOMPARE(result.size(), 1);
    QCOMPARE(QFileInfo(result[0]).fileName(), "test.bin");
}

void TestScanFiles::multiplePatterns()
{
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {"*.txt", "*.bin"});
    QCOMPARE(result.size(), 4);

    QSet<QString> names;
    for (const QString &path : result)
        names.insert(QFileInfo(path).fileName());
    QVERIFY(names.contains("file1.txt"));
    QVERIFY(names.contains("file2.txt"));
    QVERIFY(names.contains("test.bin"));
    QVERIFY(names.contains("data.bin"));
}

void TestScanFiles::nonexistentFile()
{
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {"nope.dat"});
    QCOMPARE(result.size(), 0);
}

void TestScanFiles::nonexistentDir()
{
    QStringList result = StartupDialog::scanFiles("/nonexistent/path", {"*.txt"});
    QCOMPARE(result.size(), 0);
}

void TestScanFiles::emptyPatterns()
{
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {});
    QCOMPARE(result.size(), 0);
}

void TestScanFiles::mixedPatterns()
{
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {"*.txt", "notes.md", "readme"});
    QCOMPARE(result.size(), 4);

    QSet<QString> names;
    for (const QString &path : result)
        names.insert(QFileInfo(path).fileName());
    QVERIFY(names.contains("file1.txt"));
    QVERIFY(names.contains("file2.txt"));
    QVERIFY(names.contains("notes.md"));
    QVERIFY(names.contains("readme"));
}

void TestScanFiles::noMatch()
{
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {"*.dat"});
    QCOMPARE(result.size(), 0);
}

void TestScanFiles::duplicateAvoidance()
{
    // test.bin matches both the mask and the specific name
    QStringList result = StartupDialog::scanFiles(m_tempDir->path(), {"*.bin", "test.bin"});
    QCOMPARE(result.size(), 2);

    QSet<QString> names;
    for (const QString &path : result)
        names.insert(QFileInfo(path).fileName());
    QVERIFY(names.contains("test.bin"));
    QVERIFY(names.contains("data.bin"));
}
