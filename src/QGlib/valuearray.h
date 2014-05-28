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
 * GValueArrays
 *
 * Unfortunately since this class constructs QGlib:Values to
 * return them, it can't return a reference.
 * And because they're new Values each time there's no easy way to modify them.
 */

class QTGLIB_EXPORT ValueArray
{
public:
    ValueArray();
    ValueArray(Value &value);
    explicit ValueArray(GValueArray *);
    ValueArray(QList< Value> &iterable);
    ValueArray(QVector< Value> &iterable);

    ~ValueArray();

    void append (const Value &value);
    const Value at(int i) const;
    const Value back() const;
    void clear();

    void insert( int i, const Value &value);
    int length() const;

    void prepend(const Value& value);
    void push_back(const Value &value);
    void push_front(const Value &value);

    int removeAt(int i);
    void replace(int, Value &);

    int size() const;
    // Value operator [](int);
    const Value operator[](int i) const;


    QList< Value > toList() const;
    QVector< Value > toVector() const;

    Value value(int) const;
    Value value(int, const Value &) const;

    operator GValueArray *();
    ValueArray & operator=(const ValueArray & other);
    ValueArray &operator=(GValueArray *array);

protected:
    GValueArray *garray;
};

} // namespace QGst

QGLIB_REGISTER_TYPE(QGlib::ValueArray)
#endif // QGST_VALUEARRAY_H
