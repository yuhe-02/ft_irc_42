#pragma once

template <typename T>
class IntrusivePtr {
	public:
		explicit IntrusivePtr(T* ptr = 0);
		IntrusivePtr(const IntrusivePtr &other);
		IntrusivePtr& operator=(const IntrusivePtr &other);
		~IntrusivePtr();
		T* operator->() const;
		T& operator*()  const;
		T* get()        const;
		bool operator!() const;
	private:
		T* ptr_;
};
