#ifndef QPROCESS_H
#define QPROCESS_H

#include <QIODevice>
#include <QStringList>
#include <QSharedData>

#include <functional>

QT_BEGIN_NAMESPACE

typedef qint64 Q_PID;

class QProcessEnvironmentPrivate;

class Q_CORE_EXPORT QProcessEnvironment
{
public:
    QProcessEnvironment();
    QProcessEnvironment(const QProcessEnvironment &other);
    ~QProcessEnvironment();
#ifdef Q_COMPILER_RVALUE_REFS
    QProcessEnvironment &operator=(QProcessEnvironment && other) Q_DECL_NOTHROW { swap(other); return *this; }
#endif
    QProcessEnvironment &operator=(const QProcessEnvironment &other);

    void swap(QProcessEnvironment &other) Q_DECL_NOTHROW { qSwap(d, other.d); }

    bool operator==(const QProcessEnvironment &other) const;
    inline bool operator!=(const QProcessEnvironment &other) const
    { return !(*this == other); }

    bool isEmpty() const;
    void clear();

    bool contains(const QString &name) const;
    void insert(const QString &name, const QString &value);
    void remove(const QString &name);
    QString value(const QString &name, const QString &defaultValue = QString()) const;

    QStringList toStringList() const;

    QStringList keys() const;

    void insert(const QProcessEnvironment &e);

    static QProcessEnvironment systemEnvironment();

private:
    friend class QProcessPrivate;
    friend class QProcessEnvironmentPrivate;
    QSharedDataPointer<QProcessEnvironmentPrivate> d;
};

QT_END_NAMESPACE

#endif