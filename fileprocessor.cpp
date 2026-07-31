#include "fileprocessor.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QWaitCondition>
#include <QAtomicInt>

static constexpr qint64 CHUNK_SIZE = 65536; // 64 KB

FileProcessor::FileProcessor(const QStringList &inputFiles, const QString &outputDir,
                             const QByteArray &xorKey, const QString &conflictMode,
                             bool deleteSource, QObject *parent)
    : QObject(parent)
    , m_inputFiles(inputFiles)
    , m_outputDir(outputDir)
    , m_xorKey(xorKey)
    , m_conflictMode(conflictMode)
    , m_deleteSource(deleteSource)
    , m_mutex(new QMutex)
    , m_pauseCond(new QWaitCondition)
    , m_paused(false)
{
    m_cancelled.storeRelaxed(0);
}

FileProcessor::~FileProcessor()
{
    delete m_pauseCond;
    delete m_mutex;
}

void FileProcessor::process()
{
    QDir outDir(m_outputDir);
    if (!outDir.exists() && !outDir.mkpath(".")) {
        emit error(tr("Не удалось создать директорию: %1").arg(m_outputDir));
        emit allCompleted();
        return;
    }

    for (int fileIdx = 0; fileIdx < m_inputFiles.size(); ++fileIdx) {
        if (m_cancelled.loadRelaxed())
            break;

        const QString &inPath = m_inputFiles[fileIdx];
        QFileInfo fi(inPath);

        if (!fi.exists() || !fi.isFile()) {
            emit error(tr("Файл не найден: %1").arg(inPath));
            continue;
        }

        const QString outPath = resolveOutputPath(inPath);
        QFile inFile(inPath);
        if (!inFile.open(QIODevice::ReadOnly)) {
            emit error(tr("Не удалось открыть: %1").arg(inPath));
            continue;
        }

        QFile outFile(outPath);
        if (!outFile.open(QIODevice::WriteOnly)) {
            inFile.close();
            emit error(tr("Не удалось создать: %1").arg(outPath));
            continue;
        }

        const qint64 fileSize = fi.size();
        qint64 totalRead = 0;

        while (true) {
            if (m_cancelled.loadRelaxed())
                break;

            m_mutex->lock();
            while (m_paused && !m_cancelled.loadRelaxed())
                m_pauseCond->wait(m_mutex);
            m_mutex->unlock();

            if (m_cancelled.loadRelaxed())
                break;

            QByteArray chunk = inFile.read(CHUNK_SIZE);
            if (chunk.isEmpty())
                break;

            for (int i = 0; i < chunk.size(); ++i)
                chunk[i] = chunk[i] ^ m_xorKey[i % 8];

            outFile.write(chunk);
            totalRead += chunk.size();

            emit fileProgress(fi.fileName(), totalRead, fileSize);
        }

        inFile.close();
        outFile.close();

        if (m_cancelled.loadRelaxed()) {
            outFile.remove();
            emit fileProgress(fi.fileName(), 0, fileSize);
            break;
        }

        emit fileCompleted(fi.fileName());
        emit overallProgress(fileIdx + 1, m_inputFiles.size());

        if (m_deleteSource)
            QFile::remove(inPath);
    }

    if (!m_cancelled.loadRelaxed())
        emit allCompleted();
    else
        emit error(tr("Обработка прервана пользователем"));
}

void FileProcessor::pause()
{
    QMutexLocker lock(m_mutex);
    m_paused = true;
}

void FileProcessor::resume()
{
    QMutexLocker lock(m_mutex);
    m_paused = false;
    m_pauseCond->wakeAll();
}

void FileProcessor::cancel()
{
    m_cancelled.storeRelaxed(1);
    resume(); // разбудить, если на паузе
}

QString FileProcessor::resolveOutputPath(const QString &inputPath) const
{
    QFileInfo fi(inputPath);
    QString baseName = fi.completeBaseName();
    QString ext = fi.suffix();
    if (!ext.isEmpty())
        ext = "." + ext;

    QDir outDir(m_outputDir);
    QString candidate = outDir.absoluteFilePath(fi.fileName());

    if (m_conflictMode == "overwrite")
        return candidate;

    int counter = 1;
    while (QFileInfo::exists(candidate)) {
        candidate = outDir.absoluteFilePath(baseName + "_" + QString::number(counter) + ext);
        ++counter;
    }
    return candidate;
}
