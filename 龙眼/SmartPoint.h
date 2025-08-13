#pragma once

template<typename T>
class SmartPoint
{
private:
	T* ptr;
	//void (*deleter)(T*);
public:
	T* operator->()
	{
		return ptr;
	}

	void set_ptr(T* p)
	{
		ptr = p;
	}

	SmartPoint(T* p = nullptr/*, void (*d)(T*) = [](T* p) { delete p; }*/)
		: ptr(p), deleter(d)
	{
		
	}
	~SmartPoint()
	{
		if (ptr) 
			deleter(ptr);
	}
};