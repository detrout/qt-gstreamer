/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "value.h"
#include "string.h"
#include <boost/type_traits.hpp>
#include <glib-object.h>
#include <QtCore/QDebug>
#include <QtCore/QReadWriteLock>

namespace QGlib {
namespace Private {

class Dispatcher
{
public:
    Dispatcher();

    ValueVTable getVTable(Type t) const;
    void setVTable(Type t, const ValueVTable & vtable);

private:
    mutable QReadWriteLock lock;
    QHash<Type, ValueVTable> dispatchTable;
};

Dispatcher::Dispatcher()
{
#define DECLARE_VTABLE(T, NICK, GTYPE) \
    struct ValueVTable_##NICK \
    { \
        static void get(const Value & value, void *data) \
        { \
            *reinterpret_cast<T*>(data) = g_value_get_##NICK(value); \
        }; \
        \
        static void set(Value & value, const void *data) \
        { \
            g_value_set_##NICK(value, *reinterpret_cast<T const *>(data)); \
        }; \
    }; \
    setVTable(GTYPE, ValueVTable(ValueVTable_##NICK::set, ValueVTable_##NICK::get));

    DECLARE_VTABLE(char, char, Type::Char)
    DECLARE_VTABLE(unsigned char, uchar, Type::Uchar)
    DECLARE_VTABLE(bool, boolean, Type::Boolean)
    DECLARE_VTABLE(int, int, Type::Int)
    DECLARE_VTABLE(unsigned int, uint, Type::Uint)
    DECLARE_VTABLE(long, long, Type::Long)
    DECLARE_VTABLE(unsigned long, ulong, Type::Ulong)
    DECLARE_VTABLE(qint64, int64, Type::Int64)
    DECLARE_VTABLE(quint64, uint64, Type::Uint64)
    DECLARE_VTABLE(int, enum, Type::Enum);
    DECLARE_VTABLE(uint, flags, Type::Flags)
    DECLARE_VTABLE(float, float, Type::Float)
    DECLARE_VTABLE(double, double, Type::Double)
    DECLARE_VTABLE(QByteArray, string, Type::String)
    DECLARE_VTABLE(void*, pointer, Type::Pointer)
    DECLARE_VTABLE(void*, boxed, Type::Boxed)
    DECLARE_VTABLE(GParamSpec*, param, Type::Param)
    DECLARE_VTABLE(void*, object, Type::Object)
    DECLARE_VTABLE(QGlib::Type, gtype, GetType<QGlib::Type>())

#undef DECLARE_VTABLE
}

ValueVTable Dispatcher::getVTable(Type t) const
{
    QReadLocker l(&lock);

    if (dispatchTable.contains(t)) {
        return dispatchTable[t];
    }

    while (t.isDerived()) {
        t = t.parent();
        if (dispatchTable.contains(t)) {
            return dispatchTable[t];
        }
    }

    return ValueVTable();
}

void Dispatcher::setVTable(Type t, const ValueVTable & vtable)
{
    QWriteLocker l(&lock);
    dispatchTable[t] = vtable;
}

} //namespace Private

Q_GLOBAL_STATIC(Private::Dispatcher, s_dispatcher);


Value::Value()
    : m_value(NULL)
{
}

Value::Value(const GValue *gvalue)
    : m_value(NULL)
{
    if (gvalue) {
        init(G_VALUE_TYPE(gvalue));
        g_value_copy(gvalue, m_value);
    }
}

Value::Value(Type type)
    : m_value(NULL)
{
    init(type);
}

#define VALUE_CONSTRUCTOR(T) \
    Value::Value(T val) \
        : m_value(NULL) \
    { \
        init< \
            boost::remove_const< \
                boost::remove_reference<T>::type \
            >::type \
        >(); \
        set(val); \
    }

VALUE_CONSTRUCTOR(bool)
VALUE_CONSTRUCTOR(char)
VALUE_CONSTRUCTOR(uchar)
VALUE_CONSTRUCTOR(int)
VALUE_CONSTRUCTOR(uint)
VALUE_CONSTRUCTOR(long)
VALUE_CONSTRUCTOR(ulong)
VALUE_CONSTRUCTOR(qint64)
VALUE_CONSTRUCTOR(quint64)
VALUE_CONSTRUCTOR(float)
VALUE_CONSTRUCTOR(double)
VALUE_CONSTRUCTOR(const char *)
VALUE_CONSTRUCTOR(const QByteArray &)
VALUE_CONSTRUCTOR(const QString &)

#undef VALUE_CONSTRUCTOR

Value::Value(const Value & other)
    : m_value(NULL)
{
    operator=(other);
}

Value & Value::operator=(const Value & other)
{
    if (other.isValid()) {
        init(other.type());
        g_value_copy(other, m_value);
    } else if (m_value) {
        g_value_unset(m_value);
        g_slice_free(GValue, m_value);
        m_value = NULL;
    }
    return *this;
}

Value::~Value()
{
    if (m_value) {
        g_value_unset(m_value);
        g_slice_free(GValue, m_value);
    }
}

void Value::init(Type type)
{
    if (m_value) {
        g_value_unset(m_value);
    } else {
        m_value = g_slice_new0(GValue);
    }
    g_value_init(m_value, type);
}

bool Value::isValid() const
{
    return m_value != NULL;
}

void Value::clear()
{
    if (m_value) {
        g_value_reset(m_value);
    }
}

Type Value::type() const
{
    Q_ASSERT(isValid());
    return G_VALUE_TYPE(m_value);
}

bool Value::canTransformTo(Type t) const
{
    return m_value ? g_value_type_transformable(type(), t) : false;
}

Value Value::transformTo(Type t) const
{
    Q_ASSERT(isValid());
    Value dest;
    dest.init(t);
    g_value_transform(m_value, dest.m_value);
    return dest;
}

//static
void Value::registerValueVTable(Type type, const ValueVTable & vtable)
{
    s_dispatcher()->setVTable(type, vtable);
}

void Value::getData(Type dataType, void *data) const
{
    if (!isValid()) {
        throw Private::InvalidValueException();
    } else if (g_value_type_compatible(type(), dataType)) {
        ValueVTable vtable = s_dispatcher()->getVTable(dataType);
        if (vtable.get != NULL) {
            vtable.get(*this, data);
        } else {
            throw Private::UnregisteredTypeException(dataType.name().toStdString());
        }
    } else if (dataType.isValueType() && g_value_type_transformable(type(), dataType)) {
        Value v;
        v.init(dataType);

        if (!g_value_transform(m_value, v.m_value)) {
            throw Private::TransformationFailedException(type().name().toStdString(),
                                                         dataType.name().toStdString());
        }

        v.getData(dataType, data);
    } else {
        throw Private::InvalidTypeException(dataType.name().toStdString(),
                                            type().name().toStdString());
    }
}

void Value::setData(Type dataType, const void *data)
{
    if (!isValid()) {
        throw Private::InvalidValueException();
    } else if (g_value_type_compatible(dataType, type())) {
        ValueVTable vtable = s_dispatcher()->getVTable(dataType);
        if (vtable.set != NULL) {
            vtable.set(*this, data);
        } else {
            throw Private::UnregisteredTypeException(dataType.name().toStdString());
        }
    } else if (dataType.isValueType() && g_value_type_transformable(dataType, type())) {
        Value v;
        v.init(dataType);
        v.setData(dataType, data);

        if (!g_value_transform(v.m_value, m_value)) {
            throw Private::TransformationFailedException(dataType.name().toStdString(),
                                                         type().name().toStdString());
        }
    } else {
        throw Private::InvalidTypeException(dataType.name().toStdString(),
                                            type().name().toStdString());
    }
}


QDebug & operator<<(QDebug debug, const Value & value)
{
    debug.nospace() << "QGlib::Value";
    if(!value.isValid()) {
        debug << "(<invalid>)";
        return debug.space();
    } else {
        QString str;
        if (value.type().fundamental() == QGlib::Type::String) {
            str = value.get<QString>();
        } else if (value.canTransformTo(QGlib::Type::String)) {
            str = value.transformTo(QGlib::Type::String).get<QString>();
        } else if (g_value_fits_pointer(value)) {
            quintptr ptr = reinterpret_cast<quintptr>(g_value_peek_pointer(value));
            str = QString(QLatin1String("0x%1")).arg(ptr, sizeof(quintptr)*2, 16, QLatin1Char('0'));
        } else {
            str = QLatin1String("<unknown value>");
        }

        debug << "(" << value.type().name() << ", " << str << ")";
        return debug.space();
    }
}

} //namespace QGlib
