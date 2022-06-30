#pragma once
#include <stdint.h>


// c_instance imports
#define c_instance_import \
	using c_instance::c_instance; \
	using c_instance::operator uintptr_t; \

template<typename T>
class c_instance
{
public:
	c_instance() {}
	c_instance(const uintptr_t& inst)
	{
		this->self = (T)inst;
	}
	c_instance(const T& inst) requires(!std::is_same<T, uintptr_t>)
	{
		this->self = inst;
	}
	~c_instance() {}


public:
	T self = NULL;


public: // converters
	operator uintptr_t() const { return (uintptr_t)this->self; }
	operator T() const requires(!std::is_same<T, uintptr_t>) { return this->self; }
};



template<typename T>
class c_iterator : c_instance<T*>
{
public:
	using c_instance<T*>::c_instance;

	c_iterator(T* ptr)
	{
		this->self = ptr;
	}

	c_iterator(const c_iterator& it)
	{
		this->self = it.self;
	}

public: // increment operators
	c_iterator operator++()
	{
		this->self++;
		return c_iterator<T>(*this->self);
	}

	c_iterator operator++(int)
	{
		c_iterator tmp(*this);
		operator++();
		return tmp;
	}


public: // comparison operators
	bool operator==(const c_iterator& rhs) const
	{
		return this->self == rhs.self;
	}

	bool operator!=(const c_iterator& rhs) const
	{
		return this->self != rhs.self;
	}


public: // conversions
	T& operator*()
	{
		return *this->self;
	}
};