/*

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef QGST_VALUEARRAY_H
#define QGST_VALUEARRAY_H

#include "global.h"
#include "type.h"
#include "value.h"

namespace QGlib {

/*! \headerfile valuearray.h <QGlib/valuearray.h>
 * \brief Wrapper for GValueArray
 *
 * This is a proxy class to enable interacting with
 * GValueArrays it is largely inspired by QList/QVector
 *
 * Unfortunately since this class constructs new QGlib:Values
 * intead of returning references which makes it impossilbe
 * to easily update elements of the array.
 *
 * If you do need to change a value you can use the replace
 * function.
 *
 * Additionally I only implemented a const_iterator.
 * Because iterators really need to return a pointer to an
 * existing object the iterator maintains a cached copy of
 * the value its currently looking at. This means the iterator
 * is fairly slow, and is absolutely not thread-safe.
 */

class QTGLIB_EXPORT ValueArray
{
public:

    class const_iterator {
        const ValueArray *a;
        int i;
        Value cache;

    public:
        typedef Value value_type;
        typedef Value *pointer;
        typedef Value &reference;

        inline const_iterator() : a(0), i(0) {}
        inline const_iterator(const ValueArray *v, int j=0) : a(v), i(j) {}
        inline const_iterator(const const_iterator &o) : a(o.a), i(o.i) {}
        const Value &operator[](int j);
        const Value &operator*();
        const Value *operator->();
        inline bool operator==(const const_iterator &o) const { return a == o.a && i == o.i; }
        inline bool operator!=(const const_iterator &o) const { return a != o.a || i != o.i; }
        inline bool operator<(const const_iterator &o) const { return i < o.i; }
        inline bool operator<=(const const_iterator &o) const { return i <= o.i; }
        inline bool operator>(const const_iterator &o) const { return i > o.i; }
        inline bool operator>=(const const_iterator &o) const { return i >= o.i; }
        inline const_iterator &operator++() { ++i; return *this; }
        inline const_iterator &operator--() { --i; return *this; }
        inline const_iterator &operator+=(int j) { i += j; return *this; }
        inline const_iterator &operator-=(int j) { i -= j; return *this; }

        friend class ValueArray;
    };
    friend class const_iterator;

    ValueArray();
    ValueArray(Value &value);
    ValueArray(int);
    explicit ValueArray(GValueArray *);
    ValueArray(QList< Value > &iterable);
    ValueArray(QVector< Value > &iterable);

    ~ValueArray();

    void append (const Value &value);
    const Value at(int i) const;
    const Value back() const;
    //iterator begin();
    const_iterator begin() const;
    void clear();
    const_iterator constBegin() const;
    const_iterator constEnd() const;
    // bool contains(const Value &value) const;
    // int count(const Value &value) const;
    int count() const;
    bool empty() const;
    //iterator end();
    const_iterator end() const;
    // bool endsWith(const Value &) const;
    // iterator erase(iterator pos);
    // iterator erase(iterator begin, iterator end);
    const Value first() const;
    const Value front() const;
    // int indexOf(const Value &value, int from=0) const;
    void insert( int i, const Value &value);
    bool isEmpty() const;
    const Value last() const;
    // int lastIndexOf(const Value &value, int from=-1) const;
    int length() const;
    // mid?
    void move(int from, int to);
    void pop_back();
    void pop_front();
    void prepend(const Value& value);
    void push_back(const Value &value);
    void push_front(const Value &value);
    // int removeAll(const Value &value);
    int removeAt(int i);
    void removeFirst();
    void removeLast();
    // bool removeOne(const Value &value);
    void replace(int, Value &);
    //void reserve(int alloc);
    int size() const;
    // bool startsWith(const Value &value) const;
    void swap(int i, int j);
    Value takeAt(int i);
    Value takeFirst();
    Value takeLast();
    QList< Value > toList() const;
    std::list< Value > toStdList() const;
    QVector< Value > toVector() const;
    std::vector< Value > toStdVector() const;
    Value value(int) const;
    Value value(int, const Value &) const;

    ValueArray &operator=(const ValueArray & other);
    ValueArray &operator=(GValueArray *array);
    // Value operator [](int);
    const Value operator[](int i) const;
    operator GValueArray *();

protected:
    GValueArray *garray;
};

} // namespace QGst

QGLIB_REGISTER_TYPE(QGlib::ValueArray)
#endif // QGST_VALUEARRAY_H
