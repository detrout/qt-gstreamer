#include "valuearray.h"
#include <glib-object.h>
#include <qmutex.h>

namespace QGlib {

ValueArray::ValueArray() :
  garray(0)
{
}

ValueArray::ValueArray(QGlib::Value &value) :
  garray(static_cast<GValueArray *>(g_value_get_boxed(value)))
{
}

ValueArray::ValueArray(GValueArray *value) :
  garray(value)
{
}

ValueArray::ValueArray(QList< QGlib::Value > &iterable )
{
    garray = g_value_array_new(iterable.size());
    Q_ASSERT_X(garray, "g_value_array_new", "Unable to allocate array");
    Q_FOREACH(QGlib::Value v, iterable) {
        garray = g_value_array_append(garray, v);
    }
}

ValueArray::ValueArray(QVector< QGlib::Value > &iterable )
{
    garray = g_value_array_new(iterable.size());
    Q_ASSERT_X(garray, "g_value_array_new", "Unable to allocate array");
    Q_FOREACH(QGlib::Value v, iterable) {
        garray = g_value_array_append(garray, v);
    }
}

ValueArray::~ValueArray()
{
}

void ValueArray::append( const QGlib::Value &value)
{
    Q_ASSERT_X(garray, "ValueArray::append", "Called with uninitialized array");

    garray = g_value_array_append(garray, value);
}

const QGlib::Value ValueArray::at(int i) const
{
    Q_ASSERT_X(garray, "ValueArray::at", "Called with uninitialized array");
    return QGlib::Value(g_value_array_get_nth(garray, i));
}

const QGlib::Value ValueArray::back() const
{
    return at( size() - 1 );
}

void ValueArray::clear()
{
    if (garray) {
        g_value_array_free(garray);
        garray = 0;
    }
}

void ValueArray::insert( int i, const QGlib::Value &value)
{
    Q_ASSERT_X(garray, "ValueArray::insert", "Called with uninitialized array");
    garray = g_value_array_insert(garray, i, value);
}

int ValueArray::length() const
{
    return size();
}

void ValueArray::prepend(const QGlib::Value& value)
{
    Q_ASSERT_X(garray, "ValueArray::prepend", "Called with uninitialized array");
    garray = g_value_array_prepend(garray, value);
}

void ValueArray::push_back(const QGlib::Value &value)
{
    append(value);
}

void ValueArray::push_front(const QGlib::Value &value)
{
    prepend(value);
}

int ValueArray::removeAt(int i)
{
    Q_ASSERT_X(garray, "ValueArray::removeAt", "Called with uninitialized array");

    garray = g_value_array_remove(garray, i);
}

int ValueArray::size() const
{
    if (garray) {
        return garray->n_values;
    } else {
        return 0;
    }
}

const QGlib::Value ValueArray::operator[](int i) const
{
    return at(i);
}

void ValueArray::replace(int i, QGlib::Value &source)
{
    Q_ASSERT_X(garray, "ValueArray::set", "Called with uninitialized array");

    GValue *destination = g_value_array_get_nth(garray, i);
    g_value_copy(static_cast<GValue *>(source), destination);
}

QList< QGlib::Value > ValueArray::toList() const
{
    QList< QGlib::Value > list;

    for(int i = 0; i != size(); ++i) {
        list.append( at(i) );
    }
    return list;
}

QVector< QGlib::Value > ValueArray::toVector() const
{
    QVector< QGlib::Value > vector(size());

    for(int i = 0; i != size(); ++i) {
        vector.append( at(i) );
    }
    return vector;
}

QGlib::Value ValueArray::value(int i) const
{
    if ( i >= 0 && i < size() ) {
        return QGlib::Value(g_value_array_get_nth(garray, i));
    } else {
        return QGlib::Value();
    }
}

QGlib::Value ValueArray::value(int i, const QGlib::Value &default_value) const
{
    if ( i >= 0 && i < size() ) {
        return QGlib::Value(g_value_array_get_nth(garray, i));
    } else {
        return default_value;
    }
}

ValueArray::operator GValueArray*()
{
    if (garray) {
        return garray;
    }
    return 0;
}

ValueArray & ValueArray::operator=(const ValueArray & other)
{
    garray = other.garray;
    return *this;
}

ValueArray &ValueArray::operator=(GValueArray *array)
{
    garray = array;
    return *this;
}

} //namespace QGlib
