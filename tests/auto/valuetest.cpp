/*
    Copyright (C) 2009  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
#include <QGlib/Value>
#include <QGst/Bin>
#include <QGst/Message>
#include <limits>

class ValueTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void intTest();
    void stringTest();
    void stringLiteralTest();
    void constCharTest();
    void enumTest();
    void flagsTest();
    void objectTest();
    void miniObjectTest();
    void capsTest();
    void valueTest();
    void arrayTest();
    void interfaceTest();
    void conversionsTest();
    void copyTest();
    void castTest();
    void qdebugTest();
    void datetimeTest();
    void errorTest();
};

void ValueTest::intTest()
{
    QGlib::Value v(10);
    QVERIFY(v.isValid());
    QCOMPARE(v.type(), QGlib::GetType<int>());
    QCOMPARE(v.get<int>(), 10);
}

void ValueTest::stringTest()
{
    QGlib::Value v;
    QVERIFY(!v.isValid());
    v.init<QString>();
    v.set(QString::fromUtf8("Γειά σου κόσμε"));
    QCOMPARE(v.type(), QGlib::GetType<QString>());
    QByteArray b = v.toByteArray();
    QCOMPARE(QString::fromUtf8(b), QString::fromUtf8("Γειά σου κόσμε"));
    QCOMPARE(v.get<QString>(), QString::fromUtf8("Γειά σου κόσμε"));
}

void ValueTest::stringLiteralTest()
{
    QGlib::Value v = QGlib::Value::create("Hello world");
    QCOMPARE(v.type(), QGlib::GetType<QString>());
    QCOMPARE(v.get<QString>(), QString("Hello world"));
}

void ValueTest::constCharTest()
{
    const char *foo = "Hello world";
    QGlib::Value v = QGlib::Value::create(foo);
    QCOMPARE(v.type(), QGlib::GetType<QString>());
    QCOMPARE(v.get<QString>(), QString("Hello world"));
}

void ValueTest::enumTest()
{
    QGlib::Value v;
    v.init<QGst::PadDirection>();
    v.set(QGst::PadSink);
    QVERIFY(v.isValid());
    QCOMPARE(v.type(), QGlib::GetType<QGst::PadDirection>());
    QCOMPARE(v.get<QGst::PadDirection>(), QGst::PadSink);
}

// Start of a proxy class to wrap GValueArrays with something
// that knows how to access elements & return them.
class Array
{
public:
    Array();
    Array(QGlib::Value &value);

    // I bet this should actually return a pointer.
    // or we need a different way of setting array elements.
    QGlib::Value operator[](int);

protected:
    GValueArray *data;
};

Array::Array() : data(0) {}

Array::Array(QGlib::Value &value)
{
    data = static_cast<GValueArray *>(g_value_get_boxed(value));
}

QGlib::Value Array::operator[](int i)
{
    GValue *gv = g_value_array_get_nth(this->data, i);
    return QGlib::Value(gv);
}

// Need to figure out
// QGlib::GetTypeImpl< Array >::operator Type() { return G_TYPE_VALUE_ARRAY; }
// struct ValueVTable_Array
// {
//     static void get(const QGlib::Value & value, void *data)
//     {
//     };
//
//     static void set(QGlib::Value & value, const void *data)
//     {
//     };
// };

// Create a GValueArray fill it with a few values
// And try to figure out how to extract data from it.
void ValueTest::arrayTest()
{
    // make a GValueArray with GObject calls
    GValueArray *array = g_value_array_new(2);

    GValue v0 = G_VALUE_INIT;
    QGlib::Value v1(20.02);

    g_value_init(&v0, G_TYPE_DOUBLE);
    g_value_set_double(&v0, 10.01);

    g_value_array_append(array, &v0);
    g_value_array_append(array, v1);

    GValue gvarray = G_VALUE_INIT;
    g_value_init(&gvarray, G_TYPE_VALUE_ARRAY);
    QVERIFY(G_VALUE_HOLDS(&gvarray, G_TYPE_VALUE_ARRAY));

    g_value_set_boxed(&gvarray, array);
    QVERIFY(G_VALUE_HOLDS(&gvarray, G_TYPE_VALUE_ARRAY));

    QGlib::Value qvarray(&gvarray);
    qDebug() << "qva " << qvarray;

    // pulling an element  out manually
    GValueArray *unboxed_array = static_cast<GValueArray *>(g_value_get_boxed(qvarray));

    QGlib::Value qv1(g_value_array_get_nth(unboxed_array, 1));
    QCOMPARE(qv1.get<double>(), 20.02);
    //QVERIFY(G_VALUE_HOLDS(gv, G_TYPE_VALUE_ARRAY));

    Array a1(qvarray);
    QCOMPARE(a1[0].get<double>(), 10.01);

//     QGlib::Value::registerValueVTable(QGlib::GetType<Fraction>(),
//             QGlib::ValueVTable(ValueVTable_Fraction::set, ValueVTable_Fraction::get));


}

void ValueTest::flagsTest()
{
    QGlib::Value v = QGlib::Value::create(QGst::PadFlagBlocked | QGst::PadFlagFlushing | QGst::PadFlagLast);
    QCOMPARE(v.type(), QGlib::GetType<QGst::PadFlags>());
    QCOMPARE(v.get<QGst::PadFlags>(), QGst::PadFlagBlocked | QGst::PadFlagFlushing | QGst::PadFlagLast);
}

void ValueTest::objectTest()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QGlib::Value v = QGlib::Value::create(bin);
    QCOMPARE(v.type(), QGlib::GetType<QGst::Bin>());
    QCOMPARE(static_cast<GstBin*>(v.get<QGst::BinPtr>()), static_cast<GstBin*>(bin));
}

void ValueTest::miniObjectTest()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QGst::EosMessagePtr msg = QGst::EosMessage::create(bin);
    QGlib::Value v;
    v.init<QGst::Message>();
    v.set(msg);
    QCOMPARE(v.type(), QGlib::GetType<QGst::Message>());
    QCOMPARE(static_cast<GstMessage*>(v.get<QGst::MessagePtr>()), static_cast<GstMessage*>(msg));
}

void ValueTest::capsTest()
{
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw");
    QVERIFY(caps);
    QGlib::Value v = QGlib::Value::create(caps);
    QCOMPARE(v.type(), QGlib::GetType<QGst::Caps>());
    QCOMPARE(static_cast<GstCaps*>(v.get<QGst::CapsPtr>()), static_cast<GstCaps*>(caps));
    QCOMPARE(v.get<QGst::CapsPtr>()->toString(), QString("video/x-raw"));
    QCOMPARE(v.get<QString>(), QString("video/x-raw"));
}

void ValueTest::valueTest()
{
    QGlib::Value v;
    QCOMPARE(v.isValid(), false);
    QCOMPARE(v.type(), QGlib::Type(QGlib::Type::Invalid));

    v = 2;
    QCOMPARE(v.isValid(), true);
    QCOMPARE(v.type(), QGlib::GetType<int>());

    v.set(QGlib::Value::create("foobar"));
    QCOMPARE(v.isValid(), true);
    QCOMPARE(v.type(), QGlib::GetType<const char*>());
    QCOMPARE(v.toByteArray(), QByteArray("foobar"));

    QGlib::Value v2 = v.get<QGlib::Value>();
    QCOMPARE(v2.isValid(), true);
    QCOMPARE(v2.type(), QGlib::GetType<const char*>());
    QCOMPARE(v2.toByteArray(), QByteArray("foobar"));
}

void ValueTest::interfaceTest()
{
    QGlib::Value v;
    v.init<QGst::ChildProxy>();
    QCOMPARE(v.type(), QGlib::GetType<QGst::ChildProxy>());

    QGst::ChildProxyPtr childProxy = QGst::Bin::create();
    v.set(childProxy);

    QGst::ChildProxyPtr cp2 = v.get<QGst::ChildProxyPtr>();
    QVERIFY(!cp2.isNull());
    QCOMPARE(static_cast<GstChildProxy*>(childProxy), static_cast<GstChildProxy*>(cp2));
}

void ValueTest::conversionsTest()
{
    QGlib::Value v;
    v.init<QGst::PadDirection>();

    v.set(1);
    QCOMPARE(v.get<int>(), 1);
    QCOMPARE(v.get<uint>(), 1U);
    QCOMPARE(v.get<QGst::PadDirection>(), QGst::PadSrc);
    QCOMPARE(v.get<QString>(), QString("GST_PAD_SRC"));

    v.set(2U);
    QCOMPARE(v.get<int>(), 2);
    QCOMPARE(v.get<uint>(), 2U);
    QCOMPARE(v.get<QGst::PadDirection>(), QGst::PadSink);
    QCOMPARE(v.get<QString>(), QString("GST_PAD_SINK"));


    v.init<uint>();

    v.set(100); //setting int here, not uint
    QCOMPARE(v.toInt(), 100);
    QCOMPARE(v.toUInt(), 100U);
    QCOMPARE(v.toLong(), 100L);
    QCOMPARE(v.toULong(), 100UL);
    QCOMPARE(v.toInt64(), Q_INT64_C(100));
    QCOMPARE(v.toUInt64(), Q_UINT64_C(100));
    QCOMPARE(v.toString(), QString("100"));

    v.set(-1);
    QCOMPARE(v.get<int>(), -1);
    QCOMPARE(v.get<uint>(), std::numeric_limits<uint>::max());
}

void ValueTest::copyTest()
{
    QGlib::Value v(20);

    QGlib::Value v2(v);
    QVERIFY(v2.isValid());
    QCOMPARE(v2.type(), QGlib::GetType<int>());
    QCOMPARE(v2.get<int>(), 20);

    v2.set(30);
    QCOMPARE(v.get<int>(), 20);
    QCOMPARE(v2.get<int>(), 30);

    v2 = v;
    QCOMPARE(v2.get<int>(), 20);
}

void ValueTest::castTest()
{
    QGlib::Value v(10);
    GValue *gv = v;
    QCOMPARE(G_VALUE_TYPE(gv), G_TYPE_INT);
    QCOMPARE(g_value_get_int(gv), 10);
}

void ValueTest::qdebugTest()
{
    qDebug() << QGlib::Value::create(10);
    qDebug() << QGlib::Value::create(QByteArray("Hello world"));
    qDebug() << QGlib::Value::create(QGlib::ObjectPtr());
}

void ValueTest::datetimeTest()
{
    {
        QDateTime d = QDateTime::currentDateTime();
        QGlib::Value v = QGlib::Value::create(d);
        QCOMPARE(v.get<QDateTime>(), d);
    }

    {
        GstDateTime *gstDateTime = gst_date_time_new(1.0f,
                                                     2011, 01, 16,
                                                     15, 15, 23.132);
        QGlib::Value v;
        v.init(GST_TYPE_DATE_TIME);
        g_value_take_boxed(v, gstDateTime);

        QDateTime d = v.get<QDateTime>();
        QCOMPARE(d, QDateTime(QDate(2011, 01, 16), QTime(14, 15, 23, 132), Qt::UTC));
    }

    {
        GstDateTime *gstDateTime = gst_date_time_new(-10.0f,
                                                     2011, 01, 16,
                                                     20, 50, 43.592);
        QGlib::Value v;
        v.init(GST_TYPE_DATE_TIME);
        g_value_take_boxed(v, gstDateTime);

        QDateTime d = v.get<QDateTime>();
        QCOMPARE(d, QDateTime(QDate(2011, 01, 17), QTime(6, 50, 43, 592), Qt::UTC));
    }
}

void ValueTest::errorTest()
{
    QGlib::Value v;
    QVERIFY(!v.isValid());
    v.init<QGlib::Error>();
    const QGlib::Quark domain = QGlib::Quark::fromString("test-error");
    g_value_take_boxed(v, g_error_new_literal(domain, 42, "This is a test"));
    QCOMPARE(v.type(), QGlib::GetType<QGlib::Error>());
    const QGlib::Error error = v.toError();
    QCOMPARE(error.domain().toString(), QString::fromUtf8("test-error"));
    QCOMPARE(error.message(), QString::fromUtf8("This is a test"));
    QCOMPARE(error.code(), 42);
}

QTEST_APPLESS_MAIN(ValueTest)

#include "moc_qgsttest.cpp"
#include "valuetest.moc"
