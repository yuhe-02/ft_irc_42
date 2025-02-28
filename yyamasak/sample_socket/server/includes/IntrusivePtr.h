#pragma once

template <typename T>
class IntrusivePtr
{
public:
	explicit IntrusivePtr(T* ptr = 0)
		: ptr_(ptr)
	{
		if (ptr_) {
			ptr_->addRef();
		}
	}

	IntrusivePtr(const IntrusivePtr &other)
		: ptr_(other.ptr_)
	{
		if (ptr_) {
			ptr_->addRef();
		}
	}

	IntrusivePtr& operator=(const IntrusivePtr &other)
	{
		if (this != &other) {
			if (ptr_) {
				ptr_->release();
			}
			ptr_ = other.ptr_;
			if (ptr_) {
				ptr_->addRef();
			}
		}
		return *this;
	}

	~IntrusivePtr()
	{
		if (ptr_) {
			ptr_->release();
		}
	}

	T* operator->() const { return ptr_; }
	T& operator*()  const { return *ptr_; }
	T* get()        const { return ptr_; }
	bool operator!() const { return (ptr_ == 0); }

private:
	T* ptr_;
};
