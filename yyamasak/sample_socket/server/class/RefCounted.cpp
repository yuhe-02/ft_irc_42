#include "../includes/RefCounted.h"

RefCounted::RefCounted() : ref_count_(0)
{

}

RefCounted::RefCounted(const RefCounted &other) : ref_count_(other.ref_count_)
{

}

RefCounted &RefCounted::operator=(const RefCounted &other)
{
	if (this != &other)
		ref_count_ = other.ref_count_;
	return (*this);
}

RefCounted::~RefCounted()
{

}

void RefCounted::addRef()
{
	++ref_count_;
}

void RefCounted::release()
{
	if (--ref_count_ == 0)
		delete this;
}
