#include "allocator.h"
#include <gst/gstobject.h>
#include <gst/gstminiobject.h>
#include <gst/gstallocator.h>

namespace QGst {

struct AllocatorParamsPrivate : GstAllocationParams
{
};

AllocatorParams::AllocatorParams()
  : d_ptr(new AllocatorParamsPrivate)
{
    Q_D(AllocatorParams);
    gst_allocation_params_init(d);
}

AllocatorParams::AllocatorParams(AllocatorParams &other)
  : d_ptr(static_cast<AllocatorParamsPrivate *>(gst_allocation_params_copy(other.d_ptr)))
{
}

AllocatorParams::~AllocatorParams()
{
    gst_allocation_params_free(d_ptr);
}

MemoryFlags AllocatorParams::flags() const
{
    Q_D(const AllocatorParams);
    return static_cast<QGst::MemoryFlags>(static_cast<unsigned int>(d->flags));
}

void AllocatorParams::setFlags(MemoryFlags flags)
{
    Q_D(AllocatorParams);
    d->flags = static_cast<GstMemoryFlags>(static_cast<unsigned int>(flags));
}

size_t AllocatorParams::align() const
{
    Q_D(const AllocatorParams);
    return d->align;
}

void AllocatorParams::setAlign(size_t align)
{
    Q_D(AllocatorParams);
    d->align = align;
}

size_t AllocatorParams::prefix() const
{
    Q_D(const AllocatorParams);
    return d->prefix;
}

void AllocatorParams::setPrefix(size_t align)
{
    Q_D(AllocatorParams);
    d->prefix = align;
}

size_t AllocatorParams::padding() const
{
    Q_D(const AllocatorParams);
    return d->padding;
}

void AllocatorParams::setPadding(size_t padding)
{
    Q_D(AllocatorParams);
    d->padding = padding;
}

struct AllocatorPrivate
{
     GstAllocator *g_alloc;
     int refcount;

     AllocatorPrivate() :
       g_alloc(new GstAllocator),
       refcount(0) {}

     AllocatorPrivate(GstAllocator *a) :
       g_alloc(a), refcount(1) {}

     ~AllocatorPrivate()
     {
         if (refcount == 0 && g_alloc) {
             delete g_alloc;
             g_alloc = 0;
         }
     }
};

Allocator::Allocator()
{
    m_object = static_cast<void *>(new AllocatorPrivate);
}

Allocator::Allocator(GstAllocator *alloc)
{
    m_object = static_cast<void *>(new AllocatorPrivate(alloc));
}

Allocator::~Allocator()
{
    delete static_cast<AllocatorPrivate *>(m_object);
}

GstAllocator* Allocator::object() const
{
    AllocatorPrivate *d = static_cast<AllocatorPrivate *>(m_object);
    return static_cast<GstAllocator* const>(d->g_alloc);
}

void Allocator::ref(bool increaseRef)
{
    if (increaseRef) {
        AllocatorPrivate *d = static_cast<AllocatorPrivate *>(m_object);
        ++(d->refcount);
    }
}

void Allocator::unref()
{
    AllocatorPrivate *d = static_cast<AllocatorPrivate *>(m_object);
    --(d->refcount);
}

AllocatorPtr Allocator::find(const char *name)
{
    return AllocatorPtr(new Allocator(gst_allocator_find(name)));
}

void Allocator::registerAllocator(const char *name)
{
    AllocatorPrivate *d = static_cast<AllocatorPrivate *>(m_object);
    gst_allocator_register(name, d->g_alloc);
}

void Allocator::setDefault()
{
    AllocatorPrivate *d = static_cast<AllocatorPrivate *>(m_object);

    gst_allocator_set_default(d->g_alloc);
}

MemoryPtr Allocator::alloc(size_t size, AllocatorParams &params)
{
    AllocatorPrivate *d = static_cast<AllocatorPrivate *>(m_object);

    return MemoryPtr::wrap(gst_allocator_alloc(d->g_alloc, size, params.d_ptr));
}

void Allocator::free(MemoryPtr memory)
{
    AllocatorPrivate *d = static_cast<AllocatorPrivate *>(m_object);

    gst_allocator_free(d->g_alloc, memory);
}

} /* QGst */
