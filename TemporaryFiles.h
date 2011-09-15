#ifndef TEMPORARYFILES_H
#define TEMPORARYFILES_H

#include <QHash>
#include <QVector>
#include <QByteArray>
#include <QMutex>
#include <clang-c/Index.h>

struct TemporaryFile : public CXUnsavedFile
{
    QByteArray tempFilename;
    QByteArray tempContent;
};

class TemporaryFiles
{
public:
    static TemporaryFiles* instance();

    void addFile(const QByteArray& filename, const QByteArray& content);
    void removeFile(const QByteArray& filename);

    QByteArray content(const QByteArray& filename) const;
    QVector<TemporaryFile> unsavedFiles() const;

private:
    TemporaryFiles();

    static TemporaryFiles* s_instance;

    mutable QMutex m_mutex;
    QHash<QByteArray, QByteArray> m_files;
};

#endif // TEMPORARYFILES_H
