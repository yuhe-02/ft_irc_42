#include "../includes/IntrusivePtr.hpp"

template <typename T>
IntrusivePtr<T>::IntrusivePtr(T *ptr)
	: ptr_(ptr)
{
	if (ptr_)
	{
		ptr_->addRef();
	}
}

template <typename T>
IntrusivePtr<T>::IntrusivePtr(const IntrusivePtr &other)
	: ptr_(other.ptr_)
{
	if (ptr_)
	{
		ptr_->addRef();
	}
}

template <typename T>
IntrusivePtr<T> &IntrusivePtr<T>::operator=(const IntrusivePtr &other)
{
	if (this != &other)
	{
		if (ptr_)
		{
			ptr_->release();
		}
		ptr_ = other.ptr_;
		if (ptr_)
		{
			ptr_->addRef();
		}
	}
	return *this;
}

template <typename T>
IntrusivePtr<T>::~IntrusivePtr()
{
	if (ptr_)
	{
		ptr_->release();
	}
}

template <typename T>
T *IntrusivePtr<T>::operator->() const
{
	return ptr_;
}

template <typename T>
T &IntrusivePtr<T>::operator*() const
{
	return *ptr_;
}

template <typename T>
T *IntrusivePtr<T>::get() const
{
	return ptr_;
}

template <typename T>
bool IntrusivePtr<T>::operator!() const
{
	return ptr_ == 0;
}
