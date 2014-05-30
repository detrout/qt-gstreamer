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

#define GLIB_DISABLE_DEPRECATION_WARNINGS

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
    void iteratorTest();
    void swapTest();
    void emptyTest();
    void indelTest();
};

QGlib::GetTypeImpl< QGlib::ValueArray >::operator Type() { return G_TYPE_VALUE_ARRAY; }


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

    QGlib::ValueArray a1(qvarray);
    QCOMPARE(a1[0].get<double>(), 10.01);

    QGlib::ValueArray qa(qvarray.get<QGlib::ValueArray>());
    QCOMPARE(qa[1].get<double>(), 20.02);

    QGlib::Value replacement;
    replacement.init<double>();
    replacement.set<double>(30.03);
    qa.replace(0, replacement);
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

void ValueArrayTest::iteratorTest()
{
    QVector<QGlib::Value> vector1({1,2,3,4,5});
    QGlib::ValueArray array1(vector1);

    QVector<QGlib::Value>::const_iterator vi = vector1.begin();
    QGlib::ValueArray::const_iterator ai = array1.begin();

    //test const_iterator::operator[](int)
    QCOMPARE(ai[4].toInt(), 5);
    // test const_iterator::operator*
    const QGlib::Value temp(*ai);
    QCOMPARE(temp.toInt(), 1);

    for(; vi != vector1.end(); ++vi, ++ai) {
        // test const_iterator::operator->
        QCOMPARE(vi->toInt(), ai->toInt());
    }
}

void ValueArrayTest::swapTest()
{
    QVector<QGlib::Value> vector1({1,2,3,4,5});
    QGlib::ValueArray array1(vector1);

    int last = array1.size() - 1 ;
    array1.swap(0, last);
    QCOMPARE(array1[0].toInt(), 5);
    QCOMPARE(array1.back().toInt(), 1);
}

void ValueArrayTest::emptyTest()
{
    QGlib::ValueArray array1;
    QCOMPARE(array1.isEmpty(), true);
    QCOMPARE(array1.empty(), true);
    QCOMPARE(array1.size(), 0);

    QVector<QGlib::Value> vector2({1,2,3,4,5});
    QGlib::ValueArray array2(vector2);
    QCOMPARE(array2.empty(), false);

    array2.clear();
    QCOMPARE(array2.isEmpty(), true);
}

void ValueArrayTest::indelTest()
{
    QVector<QGlib::Value> vector({1,2,3,4,5});
    QGlib::ValueArray array(vector);

    QCOMPARE(array.size(), 5);
    QCOMPARE(array.first().toInt(), 1);

    QGlib::Value zero(0);
    array.insert(0, 0);

    QCOMPARE(array.size(), 6);
    QCOMPARE(array.first().toInt(), 0);

    QGlib::Value minusone(-1);
    array.push_front(minusone);

    QCOMPARE(array.size(), 7);
    QCOMPARE(array.first().toInt(), -1);

    QGlib::Value six(6);
    array.push_back(six);

    QCOMPARE(array.size(), 8);
    QCOMPARE(array.last().toInt(), 6);

    QCOMPARE(array.takeFirst().toInt(), -1);
    QCOMPARE(array.size(), 7);

    QCOMPARE(array.takeLast().toInt(), 6);
    QCOMPARE(array.size(), 6);
}
QTEST_APPLESS_MAIN(ValueArrayTest)

#include "moc_qgsttest.cpp"
#include "valuearraytest.moc"
