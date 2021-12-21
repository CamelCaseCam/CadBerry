#pragma once
#include "CadBerry/Log.h"

/*
scoped_ptr is my own version of unique_ptr, with a couple of modifications. 

Modification 1: You can use operator= to set the pointer. If the original pointer isn't nullptr, it will be deleted. 
Modification 2: You can use scoped_ptr.raw() to "borrow" the raw pointer (you shouldn't delete it)
Modification 3: You can use scoped_ptr.forget() to remove the object's ownership of the pointer. This can cause memory leaks. 
*/

namespace CDB
{
	//Owns a raw pointer, but allows you to "lend" it to other classes/functions
	template<class T>
	class scoped_ptr
	{
	private:
		T* ptr = nullptr;

	public:
		scoped_ptr() : ptr(nullptr) {}
		scoped_ptr(T* p) : ptr(p) {}

		//Prevent copying the pointer
		scoped_ptr(const scoped_ptr& other) = delete;
		scoped_ptr& operator=(const scoped_ptr& other) = delete;

		void operator=(T* ptr)
		{
			if (this->ptr == nullptr)
				delete this->ptr;

			this->ptr = ptr;
		}

		//Allow "loaning" the raw pointer
		inline const T* raw() const { return ptr; }
		inline T* raw() { return ptr; }

		inline void forget() { ptr = nullptr; }

		//Allow moving the pointer
		scoped_ptr(scoped_ptr&& other) noexcept
		{
			this->ptr = other.ptr;
			other.ptr = nullptr;
		}

		void operator=(scoped_ptr&& other)
		{
			//Delete existing object
			delete this->ptr;

			this->ptr = other.ptr;
			other.ptr = nullptr;
		}


		//Allow dereferencing
		inline T* operator->() { return this->ptr; }

		inline T& operator*() { return *(this->ptr); }


		//Destructor to delete memory
		~scoped_ptr()
		{
			if (ptr != nullptr)
				delete ptr;
		}
	};
}