#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QAtomicInt>

class QMutex;
class QWaitCondition;

class FileProcessor : public QObject
{
    Q_OBJECT

public:
    FileProcessor(const QStringList &inputFiles, const QString &outputDir,
                  const QByteArray &xorKey, const QString &conflictMode,
                  bool deleteSource, QObject *parent = nullptr);
    ~FileProcessor() override;

public slots:
    void process();
    void pause();
    void resume();
    void cancel();

signals:
    void fileProgress(const QString &fileName, qint64 current, qint64 total);
    void overallProgress(int filesDone, int filesTotal);
    void fileCompleted(const QString &fileName);
    void allCompleted();
    void cancelled();
    void error(const QString &message);

private:
    QString resolveOutputPath(const QString &inputPath) const;

    QStringList m_inputFiles;
    QString m_outputDir;
    QByteArray m_xorKey;
    QString m_conflictMode;
    bool m_deleteSource;

    QMutex *m_mutex;
    QWaitCondition *m_pauseCond;
    bool m_paused;
    QAtomicInt m_cancelled;
};

#endif
