/*
    Copyright (C) 2014  Diane Trout <diane@ghic.org>

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

#include "qgsttest.h"
#include <QGlib/ValueArray>
#include <QGst/Bin>
#include <QGst/Message>
#include <limits>

class ValueArrayTest : public QGstTest
{
    Q_OBJECT

private Q_SLOTS:
    void arrayTest();
    void toListTest();
};

QGlib::GetTypeImpl< QGlib::ValueArray >::operator Type() { return G_TYPE_VALUE_ARRAY; }

struct ValueVTable_ValueArray
{
    //reads value and puts it in data
    static void get(const QGlib::Value & value, void *data)
    {
        GValueArray *array = static_cast<GValueArray *>(g_value_get_boxed(value));
        *reinterpret_cast<QGlib::ValueArray *>(data) = array;
    }

    // set applies data to value
    static void set(QGlib::Value & value, const void *data)
    {
        g_value_set_boxed(value, reinterpret_cast<const QGlib::ValueArray *>(data));
    };
};

// Create a GValueArray fill it with a few values
// And try to figure out how to extract data from it.
void ValueArrayTest::arrayTest()
{
    // make a GValueArray with GObject calls
    GValueArray *array = g_value_array_new(2);

    QGlib::Value v0(10.010);
    QGlib::Value v1(20.02);

    g_value_array_append(array, v0);
    g_value_array_append(array, v1);

    GValue gvarray = G_VALUE_INIT;
    g_value_init(&gvarray, G_TYPE_VALUE_ARRAY);
    QVERIFY(G_VALUE_HOLDS(&gvarray, G_TYPE_VALUE_ARRAY));

    g_value_set_boxed(&gvarray, array);
    QVERIFY(G_VALUE_HOLDS(&gvarray, G_TYPE_VALUE_ARRAY));

    QGlib::Value qvarray(&gvarray);

    // pulling an element  out manually
    GValueArray *unboxed_array = static_cast<GValueArray *>(g_value_get_boxed(qvarray));

    QGlib::Value qv1(g_value_array_get_nth(unboxed_array, 1));
    QCOMPARE(qv1.get<double>(), 20.02);
    //QVERIFY(G_VALUE_HOLDS(gv, G_TYPE_VALUE_ARRAY));

    QGlib::ValueArray a1(qvarray);
    QCOMPARE(a1[0].get<double>(), 10.01);

    QGlib::Value::registerValueVTable(QGlib::GetType<QGlib::ValueArray>(),
            QGlib::ValueVTable(ValueVTable_ValueArray::set, ValueVTable_ValueArray::get));

    QGlib::ValueArray qa(qvarray.get<QGlib::ValueArray>());
    QCOMPARE(qa[1].get<double>(), 20.02);

    QGlib::Value replacement;
    replacement.init<double>();
    replacement.set<double>(30.03);
    qa.set(0, replacement);
    QCOMPARE(qa[0].get<double>(), 30.03);

    // make sure they're the same in both views of the array
    QCOMPARE(qa[0].get<double>(),
             g_value_get_double(g_value_array_get_nth(unboxed_array, 0)));

    g_value_array_free(array);
}

void ValueArrayTest::toListTest()
{
    // make a GValueArray with GObject calls
    GValueArray *array = g_value_array_new(2);

    QGlib::Value v0(10.010);
    QGlib::Value v1(20.02);

    g_value_array_append(array, v0);
    g_value_array_append(array, v1);

    QGlib::ValueArray qvarray(array);
    // Why isn't the Qt version of size unsigned?
    QCOMPARE(static_cast<unsigned int>(qvarray.size()),
             array->n_values);

    QList< QGlib::Value > list = qvarray.toList();

    QCOMPARE(qvarray.size(), list.size());
    for(int i = 0; i != qvarray.size(); ++i) {
        QCOMPARE(qvarray[i].get<double>(), list[i].get<double>());
        QCOMPARE(list[i].get<double>(), g_value_get_double(g_value_array_get_nth(array, i)));
    }
}

QTEST_APPLESS_MAIN(ValueArrayTest)

#include "moc_qgsttest.cpp"
#include "valuearraytest.moc"
