#ifndef TEST_SCANFILES_H
#define TEST_SCANFILES_H

#include <QObject>
#include <QTemporaryDir>

class TestScanFiles : public QObject
{
    Q_OBJECT

public:
    TestScanFiles();
    ~TestScanFiles() override;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void globTxt();
    void globBin();
    void specificFile();
    void multiplePatterns();
    void nonexistentFile();
    void nonexistentDir();
    void emptyPatterns();
    void mixedPatterns();
    void noMatch();
    void duplicateAvoidance();

private:
    QTemporaryDir *m_tempDir;
};

#endif
