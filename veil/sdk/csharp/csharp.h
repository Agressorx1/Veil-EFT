#pragma once
#include <stdint.h>
#include <list>
#include <string>

#include <sdk/instance.h>
#include <sdk/game/offsets.hpp>


namespace csharp
{
	namespace structure
	{
		struct mono_object
		{
			uintptr_t vtable;		//0x00, 0
			uintptr_t sync;		//0x08, 8
		};


		struct cs_array_bounds
		{
			uint32_t length;		//0x00, 0
			uint32_t lower_bound;	//0x04, 4
		};

		struct cs_array
		{
			mono_object object;		//0x00, 0
			cs_array_bounds *bounds;	//0x10, 16
			uint32_t size;			//0x18, 24
			char _0x1C_04[0x4];		//0x1C, 28 [this is just literal padding]
			uintptr_t data;		//0x20, 32 [inlined data]
		};

		struct cs_list
		{
			char pad_0x0000[0x10];	//0x00, 0
			cs_array* items;		//0x10, 16
			int32_t size;			//0x18, 24
		};

		struct cs_string
		{
			mono_object object;		//0x00, 0
			int32_t length;		//0x10, 16
			wchar_t chars[1];		//0x14, 20
		};
	}


	template<typename T>
	class carray : public c_instance<structure::cs_array*>
	{
	public:
		c_instance_import;
		carray() {}
		~carray() {}

	public:
		inline T* data() { if (!self) return 0; return (T*)&self->data; }
		inline uint32_t size() { if (!self) return 0; return self->size; }

	public:
		T& operator[](int32_t idx)
		{
			if (!self) return 0;
			if (idx < self->size && idx >= 0)
				return (T)((uintptr_t*)&self->data)[idx];
			throw std::exception("bad csharp array index");
			return 0;
		}


	public:
		c_iterator<T> begin() { return c_iterator<T>(self ? (T*)((uintptr_t*)&self->data) : (T*)nullptr); }
		c_iterator<T> end() { return c_iterator<T>(self ? (T*)&((uintptr_t*)&self->data)[self->size] : (T*)nullptr); }
	};


	template<typename T>
	class clist : public c_instance<structure::cs_list*>
	{
	public:
		c_instance_import;
		clist() {}
		~clist() {}

	public:
		inline T* data() { if (!self) return 0; return (T*)&self->items->data; }
		inline int32_t size() { if (!self) return 0; return self->size; }

	public:
		T& operator[](int32_t idx)
		{
			if (!self)
				throw std::exception(c("csharp list is null."));
			if (idx < self->size && idx >= 0)
				return (T&)((uintptr_t*)&self->items->data)[idx];
			throw std::exception(c("bad csharp list index."));
			return (T&)((uintptr_t*)&self->items->data)[self->size - 1];
		}


		csharp::carray<T> to_array()
		{
			if (!self) return 0;
			return csharp::carray<T>((uintptr_t)self->items);
		}


	public:
		c_iterator<T> begin() { return c_iterator<T>(self ? (T*)((uintptr_t*)&self->items->data) : (T*)nullptr); }
		c_iterator<T> end() { return c_iterator<T>(self ? (T*)&((uintptr_t*)&self->items->data)[self->size] : (T*)nullptr); }
	};


	class cstring : public c_instance<structure::cs_string*>
	{
	public:
		c_instance_import;
		cstring() {}
		cstring(const cstring& str);
		cstring(const char* str);
		cstring(const std::string& str);
		~cstring() {}

	public:
		std::string c_str();

		inline wchar_t* data() { if (!self) return 0; return (wchar_t*)&self->chars; }
		inline int32_t size() { if (!self) return 0; return self->length; }

	public:
	};



	//class cobject : public c_instance<uintptr_t>
	//{
	//public:
	//	c_instance_import;
	//	cobject() {}
	//	~cobject() {}

	//public:
	//	//functions::c_sharp::object::GetHashCode
	//	
	//};

	//class cdictionary : public c_instance<uintptr_t>
	//{
	//public:
	//	c_instance_import;
	//	cdictionary() {}
	//	~cdictionary() {}

	//public:
	//	inline int32_t count() { if (!self) return 0; return *(int32_t*)(self + functions::c_sharp::dictionary::count); }
	//};if (this.buckets != null)
	//{
	//	int num = this.comparer.GetHashCode(key) & int.MaxValue;
	//	for (int i = this.buckets[num % this.buckets.Length]; i >= 0; i = this.entries[i].next)
	//	{
	//		if (this.entries[i].hashCode == num && this.comparer.Equals(this.entries[i].key, key))
	//		{
	//			return i;
	//		}
	//	}
	//}
	//return -1;
}