/*
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
#include "structs.h"
#include "miniobject.h"
#include "../QGlib/value.h"
#include <gst/gstvalue.h>
#include <gst/gstminiobject.h>

namespace QGst {
namespace Private {

void registerValueVTables()
{
    struct ValueVTable_MiniObject
    {
        static void get(const QGlib::Value & value, void *data)
        {
            *reinterpret_cast<GstMiniObject**>(data) = gst_value_get_mini_object(value);
        };

        static void set(QGlib::Value & value, const void *data)
        {
            gst_value_set_mini_object(value, *reinterpret_cast<GstMiniObject* const *>(data));
        };
    };
    QGlib::Value::registerValueVTable(QGlib::GetType<MiniObject>(),
            QGlib::ValueVTable(ValueVTable_MiniObject::set, ValueVTable_MiniObject::get));


    struct ValueVTable_Fourcc
    {
        static void get(const QGlib::Value & value, void *data)
        {
            reinterpret_cast<Fourcc*>(data)->value.as_integer = gst_value_get_fourcc(value);
        };

        static void set(QGlib::Value & value, const void *data)
        {
            gst_value_set_fourcc(value, reinterpret_cast<Fourcc const *>(data)->value.as_integer);
        };
    };
    QGlib::Value::registerValueVTable(QGlib::GetType<Fourcc>(),
            QGlib::ValueVTable(ValueVTable_Fourcc::set, ValueVTable_Fourcc::get));


    struct ValueVTable_Fraction
    {
        static void get(const QGlib::Value & value, void *data)
        {
            reinterpret_cast<Fraction*>(data)->numerator = gst_value_get_fraction_numerator(value);
            reinterpret_cast<Fraction*>(data)->denominator = gst_value_get_fraction_denominator(value);
        };

        static void set(QGlib::Value & value, const void *data)
        {
            gst_value_set_fraction(value, reinterpret_cast<Fraction const *>(data)->numerator,
                                          reinterpret_cast<Fraction const *>(data)->denominator);
        };
    };
    QGlib::Value::registerValueVTable(QGlib::GetType<Fraction>(),
            QGlib::ValueVTable(ValueVTable_Fraction::set, ValueVTable_Fraction::get));


    struct ValueVTable_IntRange
    {
        static void get(const QGlib::Value & value, void *data)
        {
            reinterpret_cast<IntRange*>(data)->start = gst_value_get_int_range_min(value);
            reinterpret_cast<IntRange*>(data)->end = gst_value_get_int_range_max(value);
        };

        static void set(QGlib::Value & value, const void *data)
        {
            gst_value_set_int_range(value, reinterpret_cast<IntRange const *>(data)->start,
                                           reinterpret_cast<IntRange const *>(data)->end);
        };
    };
    QGlib::Value::registerValueVTable(QGlib::GetType<IntRange>(),
            QGlib::ValueVTable(ValueVTable_IntRange::set, ValueVTable_IntRange::get));


    struct ValueVTable_DoubleRange
    {
        static void get(const QGlib::Value & value, void *data)
        {
            reinterpret_cast<DoubleRange*>(data)->start = gst_value_get_double_range_min(value);
            reinterpret_cast<DoubleRange*>(data)->end = gst_value_get_double_range_max(value);
        };

        static void set(QGlib::Value & value, const void *data)
        {
            gst_value_set_double_range(value, reinterpret_cast<DoubleRange const *>(data)->start,
                                              reinterpret_cast<DoubleRange const *>(data)->end);
        };
    };
    QGlib::Value::registerValueVTable(QGlib::GetType<DoubleRange>(),
            QGlib::ValueVTable(ValueVTable_DoubleRange::set, ValueVTable_DoubleRange::get));


    struct ValueVTable_FractionRange
    {
        static void get(const QGlib::Value & value, void *data)
        {
            reinterpret_cast<FractionRange*>(data)->start.numerator =
                gst_value_get_fraction_numerator(gst_value_get_fraction_range_min(value));
            reinterpret_cast<FractionRange*>(data)->start.denominator =
                gst_value_get_fraction_denominator(gst_value_get_fraction_range_min(value));
            reinterpret_cast<FractionRange*>(data)->end.numerator =
                gst_value_get_fraction_numerator(gst_value_get_fraction_range_max(value));
            reinterpret_cast<FractionRange*>(data)->end.denominator =
                gst_value_get_fraction_denominator(gst_value_get_fraction_range_max(value));
        };

        static void set(QGlib::Value & value, const void *data)
        {
            gst_value_set_fraction_range_full(value,
                    reinterpret_cast<FractionRange const *>(data)->start.numerator,
                    reinterpret_cast<FractionRange const *>(data)->start.denominator,
                    reinterpret_cast<FractionRange const *>(data)->end.numerator,
                    reinterpret_cast<FractionRange const *>(data)->end.denominator);
        };
    };
    QGlib::Value::registerValueVTable(QGlib::GetType<FractionRange>(),
            QGlib::ValueVTable(ValueVTable_FractionRange::set, ValueVTable_FractionRange::get));
}

} //namespace Private
} //namespace QGst
