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
#include <QGlib/Error>
#include <QGst/Allocator>
#include <QGst/Memory>

class AllocatorTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:

    void testAllocationParams();
    void testAllocator();
};

void AllocatorTest::testAllocationParams()
{
    QGst::AllocationParams *p(new QGst::AllocationParams);

    // its really pathetic but sometimes getters and
    // setters break
    p->setFlags(QGst::MemoryFlagReadonly | QGst::MemoryFlagNotMappable);
    p->setAlign(10ul);
    p->setPrefix(20ul);
    p->setPadding(30ul);

    QCOMPARE(p->flags(), QGst::MemoryFlagReadonly | QGst::MemoryFlagNotMappable);
    QCOMPARE(p->align(), static_cast<size_t>(10));
    QCOMPARE(p->prefix(), static_cast<size_t>(20));
    QCOMPARE(p->padding(), static_cast<size_t>(30));

    // Does copy work?
    QGst::AllocationParams c(*p);

    QCOMPARE(c.flags(), QGst::MemoryFlagReadonly | QGst::MemoryFlagNotMappable);
    QCOMPARE(c.align(), static_cast<size_t>(10));
    QCOMPARE(c.prefix(), static_cast<size_t>(20));
    QCOMPARE(c.padding(), static_cast<size_t>(30));

    // Does copy really work. (delete the source)
    delete p;

    QCOMPARE(c.flags(), QGst::MemoryFlagReadonly | QGst::MemoryFlagNotMappable);
    QCOMPARE(c.align(), static_cast<size_t>(10));
    QCOMPARE(c.prefix(), static_cast<size_t>(20));
    QCOMPARE(c.padding(), static_cast<size_t>(30));
}

void AllocatorTest::testAllocator()
{
    GstAllocator *g_system = gst_allocator_find("SystemMemory");
    QGst::AllocatorPtr system(QGst::Allocator::getSystemMemory());
    QVERIFY(system);
    QCOMPARE(g_system, static_cast<GstAllocator *>(system));

    QGst::AllocationParams params;
    params.setFlags(QGst::MemoryFlagNotMappable);

    QGst::MemoryPtr mem = system->alloc(100, params);
    QVERIFY(mem);
    QCOMPARE(mem->size(), static_cast<size_t>(100));

    system->free(mem);
}

QTEST_APPLESS_MAIN(AllocatorTest)

#include "moc_qgsttest.cpp"
#include "allocatortest.moc"
