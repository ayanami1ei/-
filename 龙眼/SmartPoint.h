#pragma once

template<typename T>
class SmartPoint
{
private:
	T* ptr;
public:
	T* operator->()
	{
		return ptr;
	}

	SmartPoint(T* p = nullptr)
		:ptr(p)
	{
		
	}
	~SmartPoint()
	{
		delete ptr;
	}
};