#pragma once

class RefCounted {
	public:
		RefCounted();
		RefCounted(const RefCounted &other);
		RefCounted &operator=(const RefCounted &other);
		virtual ~RefCounted();

		void addRef();
		void release();

	private:
		int ref_count_;
};
