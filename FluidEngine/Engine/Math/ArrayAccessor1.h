#pragma once

#include <cassert>
#include <utility>

#include "ArrayAccessor.h"
#include "../Utils/Parallel.h"

namespace Engine
{
	//!
	//! \brief 1-D array accessor class.
	//!
	//! This class represents 1-D array accessor. Array accessor provides array-like
	//! data read/write functions, but does not handle memory management. Thus, it
	//! is more like a random access iterator, but with multi-dimension support.
	//!
	//! \see Array1<T, 2>
	//!
	//! \tparam T - Array value type.
	//!
	template <typename T>
	class ArrayAccessor<T, 1> final
	{
	public:
		//! Constructs empty 1-D array accessor.
		ArrayAccessor();

		//! Constructs an array accessor that wraps given array.
		ArrayAccessor(size_t size, T* const data);

		//! Copy constructor.
		ArrayAccessor(const ArrayAccessor& other);

		//! Replaces the content with given \p other array accessor.
		void set(const ArrayAccessor& other);

		//! Resets the array.
		void reset(size_t size, T* const data);

		//! Returns the reference to the i-th element.
		T& at(size_t i);

		//! Returns the const reference to the i-th element.
		const T& at(size_t i) const;

		//! Returns the begin iterator of the array.
		T* const begin() const;

		//! Returns the end iterator of the array.
		T* const end() const;

		//! Returns the begin iterator of the array.
		T* begin();

		//! Returns the end iterator of the array.
		T* end();

		//! Returns size of the array.
		size_t size() const;

		//! Returns the raw pointer to the array data.
		T* const data() const;

		//! Swaps the content of with \p other array accessor.
		void swap(ArrayAccessor& other);

		//!
		//! \brief Iterates the array and invoke given \p func for each element.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func. The callback function takes array's element as its
		//! input. The order of execution will be 0 to N-1 where N is the size of
		//! the array. Below is the sample usage:
		//!
		//! \code{.cpp}
		//! int data = {1, 2, 3, 4, 5, 6};
		//! ArrayAccessor<int, 1> acc(6, data);
		//! acc.forEach([](int elem) {
		//!     printf("%d\n", elem);
		//! });
		//! \endcode
		//!
		template <typename Callback>
		void forEach(Callback func) const;

		//!
		//! \brief Iterates the array and invoke given \p func for each index.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func. The callback function takes one parameter which is the
		//! index of the array. The order of execution will be 0 to N-1 where N is
		//! the size of the array. Below is the sample usage:
		//!
		//! \code{.cpp}
		//! int data = {1, 2, 3, 4, 5, 6};
		//! ArrayAccessor<int, 1> acc(6, data);
		//! acc.forEachIndex([&](size_t i) {
		//!     acc[i] = 4.f * i + 1.5f;
		//! });
		//! \endcode
		//!
		template <typename Callback>
		void forEachIndex(Callback func) const;

		//!
		//! \brief Iterates the array and invoke given \p func for each element in
		//!     parallel using multi-threading.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func in parallel using multi-threading. The callback
		//! function takes array's element as its input. The order of execution will
		//! be non-deterministic since it runs in parallel.
		//! Below is the sample usage:
		//!
		//! \code{.cpp}
		//! int data = {1, 2, 3, 4, 5, 6};
		//! ArrayAccessor<int, 1> acc(6, data);
		//! acc.parallelForEach([](int& elem) {
		//!     elem *= 2;
		//! });
		//! \endcode
		//!
		//! The parameter type of the callback function doesn't have to be T&, but
		//! const T& or T can be used as well.
		//!
		template <typename Callback>
		void parallelForEach(Callback func);

		//!
		//! \brief Iterates the array and invoke given \p func for each index in
		//!     parallel using multi-threading.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func in parallel using multi-threading. The callback
		//! function takes one parameter which is the index of the array. The order
		//! of execution will be non-deterministic since it runs in parallel.
		//! Below is the sample usage:
		//!
		//! \code{.cpp}
		//! int data = {1, 2, 3, 4, 5, 6};
		//! ArrayAccessor<int, 1> acc(6, data);
		//! acc.parallelForEachIndex([](size_t i) {
		//!     acc[i] *= 2;
		//! });
		//! \endcode
		//!
		template <typename Callback>
		void parallelForEachIndex(Callback func) const;

		//! Returns the reference to i-th element.
		T& operator[](size_t i);

		//! Returns the const reference to i-th element.
		const T& operator[](size_t i) const;

		//! Copies given array accessor \p other.
		ArrayAccessor& operator=(const ArrayAccessor& other);

		//! Casts type to ConstArrayAccessor.
		operator ConstArrayAccessor<T, 1>() const;

	private:
		size_t _size;
		T* _data;
	};

	//! Type alias for 1-D array accessor.
	template <typename T> using ArrayAccessor1 = ArrayAccessor<T, 1>;

	//!
	//! \brief 1-D read-only array accessor class.
	//!
	//! This class represents 1-D read-only array accessor. Array accessor provides
	//! array-like data read/write functions, but does not handle memory management.
	//! Thus, it is more like a random access iterator, but with multi-dimension
	//! support.
	//!
	template <typename T>
	class ConstArrayAccessor<T, 1>
	{
	public:
		//! Constructs empty 1-D array accessor.
		ConstArrayAccessor();

		//! Constructs an read-only array accessor that wraps given array.
		ConstArrayAccessor(size_t size, const T* const data);

		//! Constructs a read-only array accessor from read/write accessor.
		explicit ConstArrayAccessor(const ArrayAccessor<T, 1>& other);

		//! Copy constructor.
		ConstArrayAccessor(const ConstArrayAccessor& other);

		//! Returns the const reference to the i-th element.
		const T& at(size_t i) const;

		//! Returns the begin iterator of the array.
		const T* const begin() const;

		//! Returns the end iterator of the array.
		const T* const end() const;

		//! Returns size of the array.
		size_t size() const;

		//! Returns the raw pointer to the array data.
		const T* const data() const;

		//!
		//! \brief Iterates the array and invoke given \p func for each element.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func. The callback function takes array's element as its
		//! input. The order of execution will be 0 to N-1 where N is the size of
		//! the array. Below is the sample usage:
		//!
		//! \code{.cpp}
		//! int data = {1, 2, 3, 4, 5, 6};
		//! ConstArrayAccessor<int, 1> acc(6, data);
		//! acc.forEach([](int elem) {
		//!     printf("%d\n", elem);
		//! });
		//! \endcode
		//!
		template <typename Callback>
		void forEach(Callback func) const;

		//!
		//! \brief Iterates the array and invoke given \p func for each index.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func. The callback function takes one parameter which is the
		//! index of the array. The order of execution will be 0 to N-1 where N is
		//! the size of the array. Below is the sample usage:
		//!
		//! \code{.cpp}
		//! int data = {1, 2, 3, 4, 5, 6};
		//! ConstArrayAccessor<int, 1> acc(6, data);
		//! acc.forEachIndex([&](size_t i) {
		//!     data[i] = acc[i] * acc[i];
		//! });
		//! \endcode
		//!
		template <typename Callback>
		void forEachIndex(Callback func) const;

		//!
		//! \brief Iterates the array and invoke given \p func for each index in
		//!     parallel using multi-threading.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func in parallel using multi-threading. The callback
		//! function takes one parameter which is the index of the array. The order
		//! of execution will be non-deterministic since it runs in parallel.
		//! Below is the sample usage:
		//!
		//! \code{.cpp}
		//! int data = {1, 2, 3, 4, 5, 6};
		//! ConstArrayAccessor<int, 1> acc(6, data);
		//! accessor.parallelForEachIndex([](size_t i) {
		//!     data[i] = acc[i] * acc[i];
		//! });
		//! \endcode
		//!
		template <typename Callback>
		void parallelForEachIndex(Callback func) const;

		//! Returns the const reference to i-th element.
		const T& operator[](size_t i) const;

	private:
		size_t _size;
		const T *_data;
	};

	//! Type alias for 1-D const array accessor.
	template <typename T> using ConstArrayAccessor1 = ConstArrayAccessor<T, 1>;

	//! -------------------ArrayAccessor Definition-----------------------------------
	template <typename T>
	ArrayAccessor<T, 1>::ArrayAccessor() : _size(0), _data(nullptr) { }

	template <typename T>
	ArrayAccessor<T, 1>::ArrayAccessor(size_t size, T* const data) { reset(size, data); }

	template <typename T>
	ArrayAccessor<T, 1>::ArrayAccessor(const ArrayAccessor& other) { set(other); }

	template <typename T>
	void ArrayAccessor<T, 1>::set(const ArrayAccessor& other) { reset(other._size, other._data);
	}

	template <typename T>
	void ArrayAccessor<T, 1>::reset(size_t size, T* const data) { _size = size; _data = data; }

	template <typename T>
	T& ArrayAccessor<T, 1>::at(size_t i) { assert(i < _size); return _data[i]; }

	template <typename T>
	const T& ArrayAccessor<T, 1>::at(size_t i) const { assert(i < _size); return _data[i]; }

	template <typename T>
	T* const ArrayAccessor<T, 1>::begin() const { return _data; }

	template <typename T>
	T* const ArrayAccessor<T, 1>::end() const { return _data + _size; }

	template <typename T>
	T* ArrayAccessor<T, 1>::begin() { return _data; }

	template <typename T>
	T* ArrayAccessor<T, 1>::end() { return _data + _size; }

	template <typename T>
	size_t ArrayAccessor<T, 1>::size() const { return _size; }

	template <typename T>
	T* const ArrayAccessor<T, 1>::data() const { return _data; }

	template <typename T>
	void ArrayAccessor<T, 1>::swap(ArrayAccessor& other) 
	{
		std::swap(other._data, _data);
		std::swap(other._size, _size);
	}

	template <typename T>
	template <typename Callback>
	void ArrayAccessor<T, 1>::forEach(Callback func) const 
	{
		for (size_t i = 0; i < size(); ++i) 
			func(at(i));
	}

	template <typename T>
	template <typename Callback>
	void ArrayAccessor<T, 1>::forEachIndex(Callback func) const 
	{
		for (size_t i = 0; i < size(); ++i) 
			func(i);
	}

	template <typename T>
	template <typename Callback>
	void ArrayAccessor<T, 1>::parallelForEach(Callback func) 
	{
		parallelFor(kZeroSize, size(), [&](size_t i) { func(at(i)); });
	}

	template <typename T>
	template <typename Callback>
	void ArrayAccessor<T, 1>::parallelForEachIndex(Callback func) const { parallelFor(kZeroSize, size(), func); }

	template <typename T>
	T& ArrayAccessor<T, 1>::operator[](size_t i) { return _data[i]; }

	template <typename T>
	const T& ArrayAccessor<T, 1>::operator[](size_t i) const { return _data[i]; }

	template <typename T>
	ArrayAccessor<T, 1>& ArrayAccessor<T, 1>::operator=(const ArrayAccessor& other) { set(other); return *this; }

	template <typename T>
	ArrayAccessor<T, 1>::operator ConstArrayAccessor<T, 1>() const { return ConstArrayAccessor<T, 1>(*this); }

	//! -------------------ConstArrayAccessor Definition-----------------------------------
	template <typename T>
	ConstArrayAccessor<T, 1>::ConstArrayAccessor() : _size(0), _data(nullptr) { }

	template <typename T>
	ConstArrayAccessor<T, 1>::ConstArrayAccessor(size_t size, const T* const data) { _size = size; _data = data; }

	template <typename T>
	ConstArrayAccessor<T, 1>::ConstArrayAccessor(const ArrayAccessor<T, 1>& other) { _size = other.size(); _data = other.data(); }

	template <typename T>
	ConstArrayAccessor<T, 1>::ConstArrayAccessor(const ConstArrayAccessor& other) { _size = other._size; _data = other._data; }

	template <typename T>
	const T& ConstArrayAccessor<T, 1>::at(size_t i) const { assert(i < _size); return _data[i]; }

	template <typename T>
	const T* const ConstArrayAccessor<T, 1>::begin() const { return _data; }

	template <typename T>
	const T* const ConstArrayAccessor<T, 1>::end() const { return _data + _size; }

	template <typename T>
	size_t ConstArrayAccessor<T, 1>::size() const { return _size; }

	template <typename T>
	const T* const ConstArrayAccessor<T, 1>::data() const { return _data; }

	template <typename T>
	template <typename Callback>
	void ConstArrayAccessor<T, 1>::forEach(Callback func) const 
	{
		for (size_t i = 0; i < size(); ++i) 
			func(at(i));
	}

	template <typename T>
	template <typename Callback>
	void ConstArrayAccessor<T, 1>::forEachIndex(Callback func) const 
	{
		for (size_t i = 0; i < size(); ++i)
			func(i);
	}

	template <typename T>
	template <typename Callback>
	void ConstArrayAccessor<T, 1>::parallelForEachIndex(Callback func) const 
	{
		parallelFor(kZeroSize, size(), func);
	}

	template <typename T>
	const T& ConstArrayAccessor<T, 1>::operator[](size_t i) const { return _data[i]; }

}