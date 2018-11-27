/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2016 Intel Corporation.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QProcessEnvironment>
#include <QByteArray>
#include "qprocess_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QProcessEnvironment
    \inmodule QtCore

    \brief The QProcessEnvironment class holds the environment variables that
    can be passed to a program.

    \ingroup io
    \ingroup misc
    \ingroup shared
    \reentrant
    \since 4.6

    A process's environment is composed of a set of key=value pairs known as
    environment variables. The QProcessEnvironment class wraps that concept
    and allows easy manipulation of those variables. It's meant to be used
    along with QProcess, to set the environment for child processes. It
    cannot be used to change the current process's environment.

    The environment of the calling process can be obtained using
    QProcessEnvironment::systemEnvironment().

    On Unix systems, the variable names are case-sensitive. Note that the
    Unix environment allows both variable names and contents to contain arbitrary
    binary data (except for the NUL character). QProcessEnvironment will preserve
    such variables, but does not support manipulating variables whose names or
    values cannot be encoded by the current locale settings (see
    QTextCodec::codecForLocale).

    On Windows, the variable names are case-insensitive, but case-preserving.
    QProcessEnvironment behaves accordingly.

    \sa QProcess, QProcess::systemEnvironment(), QProcess::setProcessEnvironment()
*/

QStringList QProcessEnvironmentPrivate::toList() const
{
    QStringList result;
    result.reserve(hash.size());
    for (Hash::const_iterator it = hash.cbegin(), end = hash.cend(); it != end; ++it)
        result << nameToString(it.key()) + QLatin1Char('=') + valueToString(it.value());
    return result;
}

QProcessEnvironment QProcessEnvironmentPrivate::fromList(const QStringList &list)
{
    QProcessEnvironment env;
    QStringList::ConstIterator it = list.constBegin(),
                              end = list.constEnd();
    for ( ; it != end; ++it) {
        int pos = it->indexOf(QLatin1Char('='), 1);
        if (pos < 1)
            continue;

        QString value = it->mid(pos + 1);
        QString name = *it;
        name.truncate(pos);
        env.insert(name, value);
    }
    return env;
}

QStringList QProcessEnvironmentPrivate::keys() const
{
    QStringList result;
    result.reserve(hash.size());
    Hash::ConstIterator it = hash.constBegin(),
                       end = hash.constEnd();
    for ( ; it != end; ++it)
        result << nameToString(it.key());
    return result;
}

void QProcessEnvironmentPrivate::insert(const QProcessEnvironmentPrivate &other)
{
    Hash::ConstIterator it = other.hash.constBegin(),
                       end = other.hash.constEnd();
    for ( ; it != end; ++it)
        hash.insert(it.key(), it.value());

#ifdef Q_OS_UNIX
    QHash<QString, Key>::ConstIterator nit = other.nameMap.constBegin(),
                                      nend = other.nameMap.constEnd();
    for ( ; nit != nend; ++nit)
        nameMap.insert(nit.key(), nit.value());
#endif
}

/*!
    Creates a new QProcessEnvironment object. This constructor creates an
    empty environment. If set on a QProcess, this will cause the current
    environment variables to be removed.
*/
QProcessEnvironment::QProcessEnvironment()
    : d(0)
{
}

/*!
    Frees the resources associated with this QProcessEnvironment object.
*/
QProcessEnvironment::~QProcessEnvironment()
{
}

/*!
    Creates a QProcessEnvironment object that is a copy of \a other.
*/
QProcessEnvironment::QProcessEnvironment(const QProcessEnvironment &other)
    : d(other.d)
{
}

/*!
    Copies the contents of the \a other QProcessEnvironment object into this
    one.
*/
QProcessEnvironment &QProcessEnvironment::operator=(const QProcessEnvironment &other)
{
    d = other.d;
    return *this;
}

/*!
    \fn void QProcessEnvironment::swap(QProcessEnvironment &other)
    \since 5.0

    Swaps this process environment instance with \a other. This
    function is very fast and never fails.
*/

/*!
    \fn bool QProcessEnvironment::operator !=(const QProcessEnvironment &other) const

    Returns \c true if this and the \a other QProcessEnvironment objects are different.

    \sa operator==()
*/

/*!
    Returns \c true if this and the \a other QProcessEnvironment objects are equal.

    Two QProcessEnvironment objects are considered equal if they have the same
    set of key=value pairs. The comparison of keys is done case-sensitive on
    platforms where the environment is case-sensitive.

    \sa operator!=(), contains()
*/
bool QProcessEnvironment::operator==(const QProcessEnvironment &other) const
{
    if (d == other.d)
        return true;
    if (d) {
        if (other.d) {
            QProcessEnvironmentPrivate::OrderedMutexLocker locker(d, other.d);
            return d->hash == other.d->hash;
        } else {
            return isEmpty();
        }
    } else {
        return other.isEmpty();
    }
}

/*!
    Returns \c true if this QProcessEnvironment object is empty: that is
    there are no key=value pairs set.

    \sa clear(), systemEnvironment(), insert()
*/
bool QProcessEnvironment::isEmpty() const
{
    // Needs no locking, as no hash nodes are accessed
    return d ? d->hash.isEmpty() : true;
}

/*!
    Removes all key=value pairs from this QProcessEnvironment object, making
    it empty.

    \sa isEmpty(), systemEnvironment()
*/
void QProcessEnvironment::clear()
{
    if (d)
        d->hash.clear();
    // Unix: Don't clear d->nameMap, as the environment is likely to be
    // re-populated with the same keys again.
}

/*!
    Returns \c true if the environment variable of name \a name is found in
    this QProcessEnvironment object.


    \sa insert(), value()
*/
bool QProcessEnvironment::contains(const QString &name) const
{
    if (!d)
        return false;
    QProcessEnvironmentPrivate::MutexLocker locker(d);
    return d->hash.contains(d->prepareName(name));
}

/*!
    Inserts the environment variable of name \a name and contents \a value
    into this QProcessEnvironment object. If that variable already existed,
    it is replaced by the new value.

    On most systems, inserting a variable with no contents will have the
    same effect for applications as if the variable had not been set at all.
    However, to guarantee that there are no incompatibilities, to remove a
    variable, please use the remove() function.

    \sa contains(), remove(), value()
*/
void QProcessEnvironment::insert(const QString &name, const QString &value)
{
    // our re-impl of detach() detaches from null
    d.detach(); // detach before prepareName()
    d->hash.insert(d->prepareName(name), d->prepareValue(value));
}

/*!
    Removes the environment variable identified by \a name from this
    QProcessEnvironment object. If that variable did not exist before,
    nothing happens.


    \sa contains(), insert(), value()
*/
void QProcessEnvironment::remove(const QString &name)
{
    if (d) {
        d.detach(); // detach before prepareName()
        d->hash.remove(d->prepareName(name));
    }
}

/*!
    Searches this QProcessEnvironment object for a variable identified by
    \a name and returns its value. If the variable is not found in this object,
    then \a defaultValue is returned instead.

    \sa contains(), insert(), remove()
*/
QString QProcessEnvironment::value(const QString &name, const QString &defaultValue) const
{
    if (!d)
        return defaultValue;

    QProcessEnvironmentPrivate::MutexLocker locker(d);
    QProcessEnvironmentPrivate::Hash::ConstIterator it = d->hash.constFind(d->prepareName(name));
    if (it == d->hash.constEnd())
        return defaultValue;

    return d->valueToString(it.value());
}

/*!
    Converts this QProcessEnvironment object into a list of strings, one for
    each environment variable that is set. The environment variable's name
    and its value are separated by an equal character ('=').

    The QStringList contents returned by this function are suitable for
    presentation.
    Use with the QProcess::setEnvironment function is not recommended due to
    potential encoding problems under Unix, and worse performance.

    \sa systemEnvironment(), QProcess::systemEnvironment(),
        QProcess::setProcessEnvironment()
*/
QStringList QProcessEnvironment::toStringList() const
{
    if (!d)
        return QStringList();
    QProcessEnvironmentPrivate::MutexLocker locker(d);
    return d->toList();
}

/*!
    \since 4.8

    Returns a list containing all the variable names in this QProcessEnvironment
    object.
*/
QStringList QProcessEnvironment::keys() const
{
    if (!d)
        return QStringList();
    QProcessEnvironmentPrivate::MutexLocker locker(d);
    return d->keys();
}

/*!
    \overload
    \since 4.8

    Inserts the contents of \a e in this QProcessEnvironment object. Variables in
    this object that also exist in \a e will be overwritten.
*/
void QProcessEnvironment::insert(const QProcessEnvironment &e)
{
    if (!e.d)
        return;

    // our re-impl of detach() detaches from null
    QProcessEnvironmentPrivate::MutexLocker locker(e.d);
    d->insert(*e.d);
}

QT_BEGIN_INCLUDE_NAMESPACE

extern char **environ;

QT_END_INCLUDE_NAMESPACE

QProcessEnvironment QProcessEnvironment::systemEnvironment()
{
    QProcessEnvironment env;
    const char *entry;
    for (int count = 0; (entry = environ[count]); ++count) {
        const char *equal = strchr(entry, '=');
        if (!equal)
            continue;

        QByteArray name(entry, equal - entry);
        QByteArray value(equal + 1);
        env.d->hash.insert(QProcessEnvironmentPrivate::Key(name),
                           QProcessEnvironmentPrivate::Value(value));
    }
    return env;
}

QT_END_NAMESPACE
