#include "valuearray.h"

#define GLIB_DISABLE_DEPRECATION_WARNINGS

#include <glib-object.h>
#include <qmutex.h>

namespace QGlib {

const Value &ValueArray::const_iterator::operator[](int j)
{
    Value temp(g_value_array_get_nth(const_cast<GValueArray *>(a->garray), i + j));
    cache = temp;
    return cache;
}

const Value &ValueArray::const_iterator::operator*()
{
    Value temp(g_value_array_get_nth(const_cast<GValueArray *>(a->garray), i));
    cache = temp;
    return cache;
}

const Value *ValueArray::const_iterator::operator->()
{
    Value temp(g_value_array_get_nth(const_cast<GValueArray *>(a->garray), i));
    cache = temp;
    return &cache;
}

ValueArray::ValueArray() :
  garray(0)
{
}

ValueArray::ValueArray(QGlib::Value &value) :
  garray(static_cast<GValueArray *>(g_value_get_boxed(value)))
{
}

ValueArray::ValueArray(int size) :
  garray(g_value_array_new(size))
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

ValueArray::const_iterator ValueArray::begin() const
{
    return const_iterator(this);
}

void ValueArray::clear()
{
    if (garray) {
        g_value_array_free(garray);
        garray = 0;
    }
}

ValueArray::const_iterator ValueArray::constBegin() const
{
    return begin();
}

ValueArray::const_iterator ValueArray::constEnd() const
{
    return end();
}

int ValueArray::count() const
{
    return size();
}

bool ValueArray::empty() const
{
    return isEmpty();
}

ValueArray::const_iterator ValueArray::end() const
{
    return const_iterator(this, size());
}

const Value ValueArray::first() const
{
    return at(0);
}

const Value ValueArray::front() const
{
    return first();
}

void ValueArray::insert( int i, const QGlib::Value &value)
{
    Q_ASSERT_X(garray, "ValueArray::insert", "Called with uninitialized array");
    garray = g_value_array_insert(garray, i, value);
}

bool ValueArray::isEmpty() const
{
    return size() == 0;
}

const Value ValueArray::last() const
{
    return at(size() - 1);
}

int ValueArray::length() const
{
    return size();
}

void ValueArray::pop_back()
{
    removeLast();
}

void ValueArray::pop_front()
{
    removeFirst();
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

void ValueArray::removeFirst()
{
    removeAt(0);
}

void ValueArray::removeLast()
{
    removeAt(size() - 1);
}

void ValueArray::replace(int i, QGlib::Value &source)
{
    Q_ASSERT_X(garray, "ValueArray::set", "Called with uninitialized array");

    GValue *destination = g_value_array_get_nth(garray, i);
    g_value_copy(static_cast<GValue *>(source), destination);
}

int ValueArray::size() const
{
    if (garray) {
        return garray->n_values;
    } else {
        return 0;
    }
}

void ValueArray::swap(int i, int j)
{
    GValue *a = g_value_array_get_nth(garray, i);
    GValue *b = g_value_array_get_nth(garray, j);
    GValue temp = G_VALUE_INIT;
    g_value_init(&temp, G_VALUE_TYPE(a));

    g_value_copy(a, &temp);
    g_value_copy(b, a);
    g_value_copy(&temp, b);
}

Value ValueArray::takeAt(int i)
{
    Value rv(at(i));
    removeAt(i);
    return rv;
}

Value ValueArray::takeFirst()
{
    return takeAt(0);
}

Value ValueArray::takeLast()
{
    return takeAt(size() - 1);
}

const QGlib::Value ValueArray::operator[](int i) const
{
    return at(i);
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

// Implementing these functions need a working equality test for QGlib::Values
// bool ValueArray::contains(const Value &value) const
// int ValueArray::count(const Value &value) const
// bool ValueArray::endsWith(const Value &value) const
// int ValueArray::indexOf(const Value &value, int from) const
// int ValueArray::lastIndexOf(const Value &value, int from) const
// int ValueArray::removeAll(const Value &value)
// bool ValueArray::removeOne(const Value &value)
// bool ValueArray::startsWith(const Value &value) const

} //namespace QGlib
