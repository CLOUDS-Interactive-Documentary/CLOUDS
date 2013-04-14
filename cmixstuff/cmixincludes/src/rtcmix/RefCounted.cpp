//	RefCounted.C -- Needed to allow locate for virtual table symbol.
//

#include <RefCounted.h>
#ifdef USE_OSX_DISPATCH
#include <dispatch/dispatch.h>
#endif

RefCounted::~RefCounted() {}

int RefCounted::unref()
{
	int r;
	if ((r=--_refcount) <= 0) {
#ifdef USE_OSX_DISPATCH
		dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
						^{ delete this; }
					   );
#else
		delete this;
#endif
	}
	return r;
}

void RefCounted::ref(RefCounted *r)
{
	if (r)
		r->ref();
}

// This is used to unref pointers which may be NULL without needed to do the
//	null check every time.  Just call RefCounted::Unref(ptr).

int RefCounted::unref(RefCounted *r)
{
	return (r) ? r->unref() : 0;
}
