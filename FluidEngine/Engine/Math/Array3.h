#pragma once

#include <fstream>
#include <functional>
#include <utility>
#include <vector>

#include "Array.h"
#include "Size3.h"
#include "Point3.h"
#include "ArrayAccessor3.h"

namespace Engine
{
	//!
	//! \brief 3-D array class.
	//!
	//! This class represents 3-D array data structure. Internally, the 3-D data is
	//! mapped to a linear array such that (i, j, k) element is actually stroed at
	//! (i + width * (j + height * k))th element of the linear array. This mapping
	//! means iterating i first, j and k next will give better performance such as:
	//!
	//! \code{.cpp}
	//! Array<int, 3> array;
	//! for (size_t k = 0; k < array.depth(); ++k) {
	//!     for (size_t j = 0; j < array.height(); ++j) {
	//!         for (size_t i = 0; i < array.width(); ++i) {
	//!             // Read or write array(i, j, k)
	//!         }
	//!     }
	//! }
	//! \endcode
	//!
	//! \tparam T - Type to store in the array.
	//!
	template <typename T>
	class Array<T, 3> final
	{
	public:
		typedef std::vector<T> ContainerType;
		typedef typename ContainerType::iterator Iterator;
		typedef typename ContainerType::const_iterator ConstIterator;

		//! Constructs zero-sized 3-D array.
		Array();

		//! Constructs 3-D array with given \p size and fill it with \p initVal.
		//! \param size Initial size of the array.
		//! \param initVal Initial value of each array element.
		explicit Array(const Size3& size, const T& initVal = T());

		//! Constructs 3-D array with size \p width x \p height x \p depth and fill
		//! it with \p initVal.
		//! \param width Initial width of the array.
		//! \param height Initial height of the array.
		//! \param depth Initial depth of the array.
		//! \param initVal Initial value of each array element.
		explicit Array(size_t width, size_t height, size_t depth,
			const T& initVal = T());

		//!
		//! \brief Constructs 3-D array with given initializer list \p lst.
		//!
		//! This constructor will build 3-D array with given initializer list \p lst
		//! such as
		//!
		//! \code{.cpp}
		//! Array<int, 3> arr = {
		//!     {{ 1.f,  2.f,  3.f,  4.f},
		//!      { 5.f,  6.f,  7.f,  8.f},
		//!      { 9.f, 10.f, 11.f, 12.f}},
		//!     {{13.f, 14.f, 15.f, 16.f},
		//!      {17.f, 18.f, 19.f, 20.f},
		//!      {21.f, 22.f, 23.f, 24.f}}};
		//! \endcode
		//!
		//! Note the initializer also has 3-D structure. The code above will
		//! construct 4 x 3 x 2 array.
		//!
		//! \param lst Initializer list that should be copy to the new array.
		//!
		Array(const std::initializer_list<
			std::initializer_list<std::initializer_list<T>>>& lst);

		//! Copy constructor.
		Array(const Array& other);

		//! Move constructor.
		Array(Array&& other);

		//! Sets entire array with given \p value.
		void set(const T& value);

		//! Copies given array \p other to this array.
		void set(const Array& other);

		//!
		//! Copies given initializer list \p lst to this array.
		//!
		//! This function copies given initializer list \p lst to the array such as
		//!
		//! \code{.cpp}
		//! Array<int, 3> arr;
		//! arr = {
		//!     {{ 1.f,  2.f,  3.f,  4.f},
		//!      { 5.f,  6.f,  7.f,  8.f},
		//!      { 9.f, 10.f, 11.f, 12.f}},
		//!     {{13.f, 14.f, 15.f, 16.f},
		//!      {17.f, 18.f, 19.f, 20.f},
		//!      {21.f, 22.f, 23.f, 24.f}}};
		//! \endcode
		//!
		//! Note the initializer also has 3-D structure. The code above will
		//! construct 4 x 3 x 2 array.
		//!
		//! \param lst Initializer list that should be copy to the new array.
		//!
		void set(const std::initializer_list< std::initializer_list<std::initializer_list<T>>>& lst);

		//! Clears the array and resizes to zero.
		void clear();

		//! Resizes the array with \p size and fill the new element with \p initVal.
		void resize(const Size3& size, const T& initVal = T());

		//! Resizes the array with size \p width x \p height and fill the new
		//! element with \p initVal.
		void resize(size_t width, size_t height, size_t depth, const T& initVal = T());

		//!
		//! \brief Returns the reference to the i-th element.
		//!
		//! This function returns the reference to the i-th element of the array
		//! where i is the index of linearly mapped elements such that
		//! i = x + width * (y + height * z) (x, y and z are the 3-D coordinates of
		//! the element).
		//!
		T& at(size_t i);

		//!
		//! \brief Returns the const reference to the i-th element.
		//!
		//! This function returns the reference to the i-th element of the array
		//! where i is the index of linearly mapped elements such that
		//! i = x + width * (y + height * z) (x, y and z are the 3-D coordinates of
		//! the element).
		//!
		const T& at(size_t i) const;

		//! Returns the reference to the element at (pt.x, pt.y, pt.z).
		T& at(const Point3UI& pt);

		//! Returns the const reference to the element at (pt.x, pt.y, pt.z).
		const T& at(const Point3UI& pt) const;

		//! Returns the reference to the element at (i, j, k).
		T& at(size_t i, size_t j, size_t k);

		//! Returns the const reference to the element at (i, j, k).
		const T& at(size_t i, size_t j, size_t k) const;

		//! Returns the size of the array.
		Size3 size() const;

		//! Returns the width of the array.
		size_t width() const;

		//! Returns the height of the array.
		size_t height() const;

		//! Returns the depth of the array.
		size_t depth() const;

		//! Returns the raw pointer to the array data.
		T* data();

		//! Returns the begin iterator of the array.
		Iterator begin();

		//! Returns the begin const iterator of the array.
		ConstIterator begin() const;

		//! Returns the end iterator of the array.
		Iterator end();

		//! Returns the end const iterator of the array.
		ConstIterator end() const;

		//! Returns the const raw pointer to the array data.
		const T* const data() const;

		//! Returns the array accessor.
		ArrayAccessor3<T> accessor();

		//! Returns the const array accessor.
		ConstArrayAccessor3<T> constAccessor() const;

		//! Swaps the content of the array with \p other array.
		void swap(Array& other);

		//!
		//! \brief Iterates the array and invoke given \p func for each index.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func. The callback function takes array's element as its
		//! input. The order of execution will be the same as the nested for-loop
		//! below:
		//!
		//! \code{.cpp}
		//! Array<int, 3> array;
		//! for (size_t k = 0; k < array.depth(); ++k) {
		//!     for (size_t j = 0; j < array.height(); ++j) {
		//!         for (size_t i = 0; i < array.width(); ++i) {
		//!             func(i, j, k);
		//!         }
		//!     }
		//! }
		//! \endcode
		//!
		//! Below is the sample usage:
		//!
		//! \code{.cpp}
		//! Array<int, 3> array(100, 200, 150, 4);
		//! array.forEach([](int elem) {
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
		//! function \p func. The callback function takes three parameters which are
		//! the (i, j, j) indices of the array. The order of execution will be the
		//! same as the nested for-loop below:
		//!
		//! \code{.cpp}
		//! Array<int, 3> array;
		//! for (size_t k = 0; k < array.depth(); ++k) {
		//!     for (size_t j = 0; j < array.height(); ++j) {
		//!         for (size_t i = 0; i < array.width(); ++i) {
		//!             func(i, j, k);
		//!         }
		//!     }
		//! }
		//! \endcode
		//!
		//! Below is the sample usage:
		//!
		//! \code{.cpp}
		//! Array<int, 3> array(10, 4);
		//! array.forEachIndex([&](size_t i, size_t j, size_t k) {
		//!     array(i, j, k) = 4.f * i + 7.f * j + 3.f * k + 1.5f;
		//! });
		//! \endcode
		//!
		template <typename Callback>
		void forEachIndex(Callback func) const;

		//!
		//! \brief Iterates the array and invoke given \p func for each index in
		//!     parallel.
		//!
		//! This function iterates the array elements and invoke the callback
		//! function \p func. The callback function takes array's element as its
		//! input. The order of execution will be non-deterministic since it runs in
		//! parallel. Below is the sample usage:
		//!
		//! \code{.cpp}
		//! Array<int, 3> array(100, 200, 150, 4);
		//! array.parallelForEach([](int& elem) {
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
		//! function takes two parameters which are the (i, j, k) indices of the
		//! array. The order of execution will be non-deterministic since it runs in
		//! parallel. Below is the sample usage:
		//!
		//! \code{.cpp}
		//! Array<int, 3> array(100, 200, 150, 4);
		//! array.parallelForEachIndex([&](size_t i, size_t j, size_t k) {
		//!     array(i, j, k) *= 2;
		//! });
		//! \endcode
		//!
		template <typename Callback>
		void parallelForEachIndex(Callback func) const;

		//!
		//! \brief Returns the reference to the i-th element.
		//!
		//! This function returns the reference to the i-th element of the array
		//! where i is the index of linearly mapped elements such that
		//! i = x + width * (y + height * z) (x, y and z are the 3-D coordinates of
		//! the element).
		//!
		//! \see Array<T, 2>::at
		//!
		T& operator[](size_t i);

		//!
		//! \brief Returns the const reference to the i-th element.
		//!
		//! This function returns the const reference to the i-th element of the
		//! array where i is the index of linearly mapped elements such that
		//! i = x + width * (y + height * z) (x, y and z are the 3-D coordinates of
		//! the element).
		//!
		//! \see Array<T, 2>::at
		//!
		const T& operator[](size_t i) const;

		//! Returns the reference to the element at (pt.x, pt.y, pt.z).
		T& operator()(const Point3UI& pt);

		//! Returns the const reference to the element at (pt.x, pt.y, pt.z).
		const T& operator()(const Point3UI& pt) const;

		//! Returns the reference to the element at (i, j, k).
		T& operator()(size_t i, size_t j, size_t k);

		//! Returns the const reference to the element at (i, j, k).
		const T& operator()(size_t i, size_t j, size_t k) const;

		//! Sets entire array with given \p value.
		Array& operator=(const T& value);

		//! Copies given array \p other to this array.
		Array& operator=(const Array& other);

		//! Move assignment.
		Array& operator=(Array&& other);

		//!
		//! Copies given initializer list \p lst to this array.
		//!
		//! This function copies given initializer list \p lst to the array such as
		//!
		//! \code{.cpp}
		//! Array<int, 3> arr;
		//! arr = {
		//!     {{ 1.f,  2.f,  3.f,  4.f},
		//!      { 5.f,  6.f,  7.f,  8.f},
		//!      { 9.f, 10.f, 11.f, 12.f}},
		//!     {{13.f, 14.f, 15.f, 16.f},
		//!      {17.f, 18.f, 19.f, 20.f},
		//!      {21.f, 22.f, 23.f, 24.f}}};
		//! \endcode
		//!
		//! Note the initializer also has 3-D structure. The code above will
		//! construct 4 x 3 x 2 array.
		//!
		//! \param lst Initializer list that should be copy to the new array.
		//!
		Array& operator=(const std::initializer_list<
			std::initializer_list<std::initializer_list<T>>>& lst);

	private:
		Size3 _size;
		std::vector<T> _data;
	};

	//! Type alias for 3-D array.
	template <typename T>
	using Array3 = Array<T, 3>;

	template <typename T>
	Array<T, 3>::Array() {}

	template <typename T>
	Array<T, 3>::Array(const Size3& size, const T& initVal) { resize(size, initVal); }

	template <typename T>
	Array<T, 3>::Array(size_t width, size_t height, size_t depth, const T& initVal) { resize(width, height, depth, initVal); }

	template <typename T>
	Array<T, 3>::Array(const std::initializer_list<std::initializer_list<std::initializer_list<T>>>& lst) { set(lst); }

	template <typename T>
	Array<T, 3>::Array(const Array& other) { set(other); }

	template <typename T>
	Array<T, 3>::Array(Array&& other) { (*this) = std::move(other); }

	template <typename T>
	void Array<T, 3>::set(const T& value) 
	{
		for (auto& v : _data) 
			v = value;
	}

	template <typename T>
	void Array<T, 3>::set(const Array& other) 
	{
		_data.resize(other._data.size());
		std::copy(other._data.begin(), other._data.end(), _data.begin());
		_size = other._size;
	}

	template <typename T>
	void Array<T, 3>::set(const std::initializer_list< std::initializer_list<std::initializer_list<T>>>& lst) 
	{
		size_t depth = lst.size();
		auto pageIter = lst.begin();
		size_t height = (depth > 0) ? pageIter->size() : 0;
		auto rowIter = pageIter->begin();
		size_t width = (height > 0) ? rowIter->size() : 0;
		resize(Size3(width, height, depth));
		for (size_t k = 0; k < depth; ++k) 
		{
			rowIter = pageIter->begin();
			assert(height == pageIter->size());
			for (size_t j = 0; j < height; ++j) 
			{
				auto colIter = rowIter->begin();
				assert(width == rowIter->size());
				for (size_t i = 0; i < width; ++i) 
				{
					(*this)(i, j, k) = *colIter;
					++colIter;
				}
				++rowIter;
			}
			++pageIter;
		}
	}

	template <typename T>
	void Array<T, 3>::clear() 
	{
		_size = Size3(0, 0, 0);
		_data.clear();
	}

	template <typename T>
	void Array<T, 3>::resize(const Size3& size, const T& initVal) {
		Array grid;
		grid._data.resize(size.x * size.y * size.z, initVal);
		grid._size = size;
		size_t iMin = std::min(size.x, _size.x);
		size_t jMin = std::min(size.y, _size.y);
		size_t kMin = std::min(size.z, _size.z);
		for (size_t k = 0; k < kMin; ++k) 
			for (size_t j = 0; j < jMin; ++j) 
				for (size_t i = 0; i < iMin; ++i) 
					grid(i, j, k) = at(i, j, k);
		swap(grid);
	}

	template <typename T>
	void Array<T, 3>::resize(size_t width, size_t height, size_t depth, const T& initVal) 
	{
		resize(Size3(width, height, depth), initVal);
	}

	template <typename T>
	T& Array<T, 3>::at(size_t i) 
	{
		assert(i < _size.x * _size.y * _size.z);
		return _data[i];
	}

	template <typename T>
	const T& Array<T, 3>::at(size_t i) const 
	{
		assert(i < _size.x * _size.y * _size.z);
		return _data[i];
	}

	template <typename T>
	T& Array<T, 3>::at(const Point3UI& pt)  { return at(pt.x, pt.y, pt.z); }

	template <typename T>
	const T& Array<T, 3>::at(const Point3UI& pt) const { return at(pt.x, pt.y, pt.z); }

	template <typename T>
	T& Array<T, 3>::at(size_t i, size_t j, size_t k) 
	{
		assert(i < _size.x && j < _size.y && k < _size.z);
		return _data[i + _size.x * (j + _size.y * k)];
	}

	template <typename T>
	const T& Array<T, 3>::at(size_t i, size_t j, size_t k) const 
	{
		assert(i < _size.x && j < _size.y && k < _size.z);
		return _data[i + _size.x * (j + _size.y * k)];
	}

	template <typename T>
	Size3 Array<T, 3>::size() const { return _size; }

	template <typename T>
	size_t Array<T, 3>::width() const { return _size.x; }

	template <typename T>
	size_t Array<T, 3>::height() const { return _size.y; }

	template <typename T>
	size_t Array<T, 3>::depth() const { return _size.z; }

	template <typename T>
	T* Array<T, 3>::data() { return _data.data(); }

	template <typename T>
	const T* const Array<T, 3>::data() const { return _data.data(); }

	template <typename T>
	typename Array<T, 3>::ContainerType::iterator Array<T, 3>::begin() { return _data.begin(); }

	template <typename T>
	typename Array<T, 3>::ContainerType::const_iterator Array<T, 3>::begin() const { return _data.cbegin(); }

	template <typename T>
	typename Array<T, 3>::ContainerType::iterator Array<T, 3>::end() { return _data.end(); }

	template <typename T>
	typename Array<T, 3>::ContainerType::const_iterator Array<T, 3>::end() const { return _data.cend(); }

	template <typename T>
	ArrayAccessor3<T> Array<T, 3>::accessor() { return ArrayAccessor3<T>(size(), data()); }

	template <typename T>
	ConstArrayAccessor3<T> Array<T, 3>::constAccessor() const { return ConstArrayAccessor3<T>(size(), data()); }

	template <typename T>
	void Array<T, 3>::swap(Array& other) { std::swap(other._data, _data); std::swap(other._size, _size); }

	template <typename T>
	template <typename Callback>
	void Array<T, 3>::forEach(Callback func) const 
	{
		constAccessor().forEach(func);
	}

	template <typename T>
	template <typename Callback>
	void Array<T, 3>::forEachIndex(Callback func) const 
	{
		constAccessor().forEachIndex(func);
	}

	template <typename T>
	template <typename Callback>
	void Array<T, 3>::parallelForEach(Callback func) 
	{
		accessor().parallelForEach(func);
	}

	template <typename T>
	template <typename Callback>
	void Array<T, 3>::parallelForEachIndex(Callback func) const 
	{
		constAccessor().parallelForEachIndex(func);
	}

	template <typename T>
	T& Array<T, 3>::operator[](size_t i) { return _data[i]; }

	template <typename T>
	const T& Array<T, 3>::operator[](size_t i) const { return _data[i]; }

	template <typename T>
	T& Array<T, 3>::operator()(size_t i, size_t j, size_t k) 
	{
		assert(i < _size.x && j < _size.y && k < _size.z);
		return _data[i + _size.x * (j + _size.y * k)];
	}

	template <typename T>
	const T& Array<T, 3>::operator()(size_t i, size_t j, size_t k) const 
	{
		assert(i < _size.x && j < _size.y && k < _size.z);
		return _data[i + _size.x * (j + _size.y * k)];
	}

	template <typename T>
	T& Array<T, 3>::operator()(const Point3UI& pt) 
	{
		assert(pt.x < _size.x && pt.y < _size.y && pt.z < _size.z);
		return _data[pt.x + _size.x * (pt.y + _size.y * pt.z)];
	}

	template <typename T>
	const T& Array<T, 3>::operator()(const Point3UI& pt) const 
	{
		assert(pt.x < _size.x && pt.y < _size.y && pt.z < _size.z);
		return _data[pt.x + _size.x * (pt.y + _size.y * pt.z)];
	}

	template <typename T>
	Array<T, 3>& Array<T, 3>::operator=(const T& value) { set(value); return *this; }

	template <typename T>
	Array<T, 3>& Array<T, 3>::operator=(const Array& other) { set(other); return *this; }

	template <typename T>
	Array<T, 3>& Array<T, 3>::operator=(Array&& other) 
	{
		_data = std::move(other._data);
		_size = other._size;
		other._size = Size3();
		return *this;
	}

	template <typename T>
	Array<T, 3>& Array<T, 3>::operator=(const std::initializer_list<std::initializer_list<std::initializer_list<T>>>& lst)
	{
		set(lst);
		return *this;
	}

}