#include "qprocess.h"

#include <QStringList>
#include <QHash>
#include <QSharedData>
#include <QMutex>

class QProcEnvKey
{
public:
    QProcEnvKey() : hash(0) {}
    explicit QProcEnvKey(const QByteArray &other) : key(other), hash(qHash(key)) {}
    QProcEnvKey(const QProcEnvKey &other) { *this = other; }
    bool operator==(const QProcEnvKey &other) const { return key == other.key; }

    QByteArray key;
    uint hash;
};
inline uint qHash(const QProcEnvKey &key) Q_DECL_NOTHROW { return key.hash; }

class QProcEnvValue
{
public:
    QProcEnvValue() {}
    QProcEnvValue(const QProcEnvValue &other) { *this = other; }
    explicit QProcEnvValue(const QString &value) : stringValue(value) {}
    explicit QProcEnvValue(const QByteArray &value) : byteValue(value) {}
    bool operator==(const QProcEnvValue &other) const
    {
        return byteValue.isEmpty() && other.byteValue.isEmpty()
                ? stringValue == other.stringValue
                : bytes() == other.bytes();
    }
    QByteArray bytes() const
    {
        if (byteValue.isEmpty() && !stringValue.isEmpty())
            byteValue = stringValue.toLocal8Bit();
        return byteValue;
    }
    QString string() const
    {
        if (stringValue.isEmpty() && !byteValue.isEmpty())
            stringValue = QString::fromLocal8Bit(byteValue);
        return stringValue;
    }

    mutable QByteArray byteValue;
    mutable QString stringValue;
};
Q_DECLARE_TYPEINFO(QProcEnvValue, Q_MOVABLE_TYPE);

Q_DECLARE_TYPEINFO(QProcEnvKey, Q_MOVABLE_TYPE);

class QProcessEnvironmentPrivate: public QSharedData
{
public:
    typedef QProcEnvKey Key;
    typedef QProcEnvValue Value;

	inline Key prepareName(const QString &name) const
    {
        Key &ent = nameMap[name];
        if (ent.key.isEmpty())
            ent = Key(name.toLocal8Bit());
        return ent;
    }
    inline QString nameToString(const Key &name) const
    {
        const QString sname = QString::fromLocal8Bit(name.key);
        nameMap[sname] = name;
        return sname;
    }
    inline Value prepareValue(const QString &value) const { return Value(value); }
    inline QString valueToString(const Value &value) const { return value.string(); }

    struct MutexLocker : public QMutexLocker
    {
        MutexLocker(const QProcessEnvironmentPrivate *d) : QMutexLocker(&d->mutex) {}
    };
    struct OrderedMutexLocker
    {
		QMutex *mutex1;
		QMutex *mutex2;
        OrderedMutexLocker(const QProcessEnvironmentPrivate *d1,
                           const QProcessEnvironmentPrivate *d2) :
            mutex1(&d1->mutex), mutex2(&d2->mutex)
        {
			if (mutex2 == mutex1)
				mutex2 = 0;

			if (mutex1) mutex1->lock();
			if (mutex2) mutex2->lock();
		}
		~OrderedMutexLocker()
		{
			if (mutex2) mutex2->unlock();
			if (mutex1) mutex1->unlock();
		}
    };

    QProcessEnvironmentPrivate() : QSharedData() {}
    QProcessEnvironmentPrivate(const QProcessEnvironmentPrivate &other) :
        QSharedData()
    {
        // This being locked ensures that the functions that only assign
        // d pointers don't need explicit locking.
        // We don't need to lock our own mutex, as this object is new and
        // consequently not shared. For the same reason, non-const methods
        // do not need a lock, as they detach objects (however, we need to
        // ensure that they really detach before using prepareName()).
        MutexLocker locker(&other);
        hash = other.hash;
        nameMap = other.nameMap;
        // We need to detach our members, so that our mutex can protect them.
        // As we are being detached, they likely would be detached a moment later anyway.
        hash.detach();
        nameMap.detach();
    }

    typedef QHash<Key, Value> Hash;
    Hash hash;

    typedef QHash<QString, Key> NameHash;
    mutable NameHash nameMap;

    mutable QMutex mutex;

    static QProcessEnvironment fromList(const QStringList &list);
    QStringList toList() const;
    QStringList keys() const;
    void insert(const QProcessEnvironmentPrivate &other);
};

template<> Q_INLINE_TEMPLATE void QSharedDataPointer<QProcessEnvironmentPrivate>::detach()
{
    if (d && d->ref.load() == 1)
        return;
    QProcessEnvironmentPrivate *x = (d ? new QProcessEnvironmentPrivate(*d)
                                     : new QProcessEnvironmentPrivate);
    x->ref.ref();
    if (d && !d->ref.deref())
        delete d;
    d = x;
}
