#pragma once

#include <tuple>

#include "Size2.h"
#include "VectorN.h"
#include "../Utils/Parallel.h"
#include "../Utils/MathUtils.h"
#include "MatrixExpression.h"

#include <numeric>

namespace Engine
{

	template <typename T>
	class MatrixCsr;

	//!
	//! \brief Vector expression for CSR matrix-vector multiplication.
	//!
	//! This vector expression represents a CSR matrix-vector operation that
	//! takes one CSR input matrix expression and one vector expression.
	//!
	//! \tparam T   Element value type.
	//! \tparam VE  Vector expression.
	//!
	template <typename T, typename VE>
	class MatrixCsrVectorMul : public VectorExpression<T, MatrixCsrVectorMul<T, VE>>
	{
	public:
		MatrixCsrVectorMul(const MatrixCsr<T>& m, const VE& v);
		MatrixCsrVectorMul(const MatrixCsrVectorMul&);

		//! Size of the vector.
		size_t size() const;

		//! Returns vector element at i.
		T operator[](size_t i) const;

	private:
		const MatrixCsr<T>& _m;
		const VE& _v;
		const VE* _v2;

	};

	//!
	//! \brief Matrix expression for CSR matrix-matrix multiplication.
	//!
	//! This matrix expression represents a CSR matrix-matrix operation that
	//! takes one CSR input matrix expression and one (probably dense) matrix
	//! expression.
	//!
	//! \tparam T   Element value type.
	//! \tparam ME  Matrix expression.
	//!
	template <typename T, typename ME>
	class MatrixCsrMatrixMul : public MatrixExpression<T, MatrixCsrMatrixMul<T, ME>>
	{
	public:
		MatrixCsrMatrixMul(const MatrixCsr<T>& m1, const ME& m2);

		//! Size of the matrix.
		Size2 size() const;

		//! Number of rows.
		size_t rows() const;

		//! Number of columns.
		size_t cols() const;

		//! Returns matrix element at (i, j).
		T operator()(size_t i, size_t j) const;

	private:
		const MatrixCsr<T>& _m1;
		const ME& _m2;
		const T* const _nnz;
		const size_t* const _rp;
		const size_t* const _ci;
	};

	//!
	//! \brief Compressed Sparse Row (CSR) matrix class.
	//!
	//! This class defines Compressed Sparse Row (CSR) matrix using arrays of
	//! non-zero elements, row pointers, and column indices.
	//!
	//! \see http://www.netlib.org/utk/people/JackDongarra/etemplates/node373.html
	//!
	//! \tparam T Type of the element.
	//!
	template <typename T>
	class MatrixCsr final : public MatrixExpression<T, MatrixCsr<T>>
	{
	public:
		static_assert(
			std::is_floating_point<T>::value,
			"MatrixCsr only can be instantiated with floating point types");

		struct Element 
		{
			size_t i;
			size_t j;
			T value;
			Element();
			Element(size_t i, size_t j, const T& value);
		};

		typedef std::vector<T> NonZeroContainerType;
		typedef typename NonZeroContainerType::iterator NonZeroIterator;
		typedef typename NonZeroContainerType::const_iterator ConstNonZeroIterator;

		typedef std::vector<size_t> IndexContainerType;
		typedef typename IndexContainerType::iterator IndexIterator;
		typedef typename IndexContainerType::const_iterator ConstIndexIterator;

		// MARK: Constructors

		//! Constructs an empty matrix.
		MatrixCsr();

		//!
		//! \brief Compresses given initializer list \p lst into a sparse matrix.
		//!
		//! This constructor will build a matrix with given initializer list \p lst
		//! such as
		//!
		//! \code{.cpp}
		//! MatrixCsr<float> mat = {
		//!     {1.f, 0.f, 0.f, 3.f},
		//!     {0.f, 3.f, 5.f, 1.f},
		//!     {4.f, 0.f, 1.f, 5.f}
		//! };
		//! \endcode
		//!
		//! Note the initializer has 4x3 structure which will create 4x3 matrix.
		//! During the process, zero elements (less than \p epsilon) will not be
		//! stored.
		//!
		//! \param lst Initializer list that should be copy to the new matrix.
		//!
		MatrixCsr(const std::initializer_list<std::initializer_list<T>>& lst,
			T epsilon = std::numeric_limits<T>::epsilon());

		//!
		//! \brief Compresses input (dense) matrix expression into a sparse matrix.
		//!
		//! This function sets this sparse matrix with dense input matrix.
		//! During the process, zero elements (less than \p epsilon) will not be
		//! stored.
		//!
		template <typename E>
		MatrixCsr(const MatrixExpression<T, E>& other,
			T epsilon = std::numeric_limits<T>::epsilon());

		//! Copy constructor.
		MatrixCsr(const MatrixCsr& other);

		//! Move constructor.
		MatrixCsr(MatrixCsr&& other);

		// MARK: Basic setters

		//! Clears the matrix and make it zero-dimensional.
		void clear();

		//! Sets whole matrix with input scalar.
		void set(const T& s);

		//! Copy from given sparse matrix.
		void set(const MatrixCsr& other);

		//! Reserves memory space of this matrix.
		void reserve(size_t rows, size_t cols, size_t numNonZeros);

		//!
		//! \brief Compresses given initializer list \p lst into a sparse matrix.
		//!
		//! This function will fill the matrix with given initializer list \p lst
		//! such as
		//!
		//! \code{.cpp}
		//! MatrixCsr<float> mat;
		//! mat.compress({
		//!     {1.f, 0.f, 0.f, 3.f},
		//!     {0.f, 3.f, 5.f, 1.f},
		//!     {4.f, 0.f, 1.f, 5.f}
		//! });
		//! \endcode
		//!
		//! Note the initializer has 4x3 structure which will resize to 4x3 matrix.
		//! During the process, zero elements (less than \p epsilon) will not be
		//! stored.
		//!
		//! \param lst Initializer list that should be copy to the new matrix.
		//!
		void compress(const std::initializer_list<std::initializer_list<T>>& lst,
			T epsilon = std::numeric_limits<T>::epsilon());

		//!
		//! \brief Compresses input (dense) matrix expression into a sparse matrix.
		//!
		//! This function sets this sparse matrix with dense input matrix.
		//! During the process, zero elements (less than \p epsilon) will not be
		//! stored.
		//!
		template <typename E>
		void compress(const MatrixExpression<T, E>& other,
			T epsilon = std::numeric_limits<T>::epsilon());

		//! Adds non-zero element to (i, j).
		void addElement(size_t i, size_t j, const T& value);

		//! Adds non-zero element.
		void addElement(const Element& element);

		//!
		//! Adds a row to the sparse matrix.
		//!
		//! \param nonZeros - Array of non-zero elements for the row.
		//! \param columnIndices - Array of column indices for the row.
		//!
		void addRow(const NonZeroContainerType& nonZeros,
			const IndexContainerType& columnIndices);

		//! Sets non-zero element to (i, j).
		void setElement(size_t i, size_t j, const T& value);

		//! Sets non-zero element.
		void setElement(const Element& element);

		// MARK: Basic getters
		bool isEqual(const MatrixCsr& other) const;

		//! Returns true if this matrix is similar to the input matrix within the
		//! given tolerance.
		bool isSimilar(const MatrixCsr& other,
			double tol = std::numeric_limits<double>::epsilon()) const;

		//! Returns true if this matrix is a square matrix.
		bool isSquare() const;

		//! Returns the size of this matrix.
		Size2 size() const;

		//! Returns number of rows of this matrix.
		size_t rows() const;

		//! Returns number of columns of this matrix.
		size_t cols() const;

		//! Returns the number of non-zero elements.
		size_t numberOfNonZeros() const;

		//! Returns i-th non-zero element.
		const T& nonZero(size_t i) const;

		//! Returns i-th non-zero element.
		T& nonZero(size_t i);

		//! Returns i-th row pointer.
		const size_t& rowPointer(size_t i) const;

		//! Returns i-th column index.
		const size_t& columnIndex(size_t i) const;

		//! Returns pointer of the non-zero elements data.
		T* nonZeroData();

		//! Returns constant pointer of the non-zero elements data.
		const T* const nonZeroData() const;

		//! Returns constant pointer of the row pointers data.
		const size_t* const rowPointersData() const;

		//! Returns constant pointer of the column indices data.
		const size_t* const columnIndicesData() const;

		//! Returns the begin iterator of the non-zero elements.
		NonZeroIterator nonZeroBegin();

		//! Returns the begin const iterator of the non-zero elements.
		ConstNonZeroIterator nonZeroBegin() const;

		//! Returns the end iterator of the non-zero elements.
		NonZeroIterator nonZeroEnd();

		//! Returns the end const iterator of the non-zero elements.
		ConstNonZeroIterator nonZeroEnd() const;

		//! Returns the begin iterator of the row pointers.
		IndexIterator rowPointersBegin();

		//! Returns the begin const iterator of the row pointers.
		ConstIndexIterator rowPointersBegin() const;

		//! Returns the end iterator of the row pointers.
		IndexIterator rowPointersEnd();

		//! Returns the end const iterator of the row pointers.
		ConstIndexIterator rowPointersEnd() const;

		//! Returns the begin iterator of the column indices.
		IndexIterator columnIndicesBegin();

		//! Returns the begin const iterator of the column indices.
		ConstIndexIterator columnIndicesBegin() const;

		//! Returns the end iterator of the column indices.
		IndexIterator columnIndicesEnd();

		//! Returns the end const iterator of the column indices.
		ConstIndexIterator columnIndicesEnd() const;

		// MARK: Binary operator methods - new instance = this instance (+) input

		//! Returns this matrix + input scalar.
		MatrixCsr add(const T& s) const;

		//! Returns this matrix + input matrix (element-wise).
		MatrixCsr add(const MatrixCsr& m) const;

		//! Returns this matrix - input scalar.
		MatrixCsr sub(const T& s) const;

		//! Returns this matrix - input matrix (element-wise).
		MatrixCsr sub(const MatrixCsr& m) const;

		//! Returns this matrix * input scalar.
		MatrixCsr mul(const T& s) const;

		//! Returns this matrix * input vector.
		template <typename VE>
		MatrixCsrVectorMul<T, VE> mul(const VectorExpression<T, VE>& v) const;

		//! Returns this matrix * input matrix.
		template <typename ME>
		MatrixCsrMatrixMul<T, ME> mul(const MatrixExpression<T, ME>& m) const;

		//! Returns this matrix / input scalar.
		MatrixCsr div(const T& s) const;

		// MARK: Binary operator methods - new instance = input (+) this instance

		//! Returns input scalar + this matrix.
		MatrixCsr radd(const T& s) const;

		//! Returns input matrix + this matrix (element-wise).
		MatrixCsr radd(const MatrixCsr& m) const;

		//! Returns input scalar - this matrix.
		MatrixCsr rsub(const T& s) const;

		//! Returns input matrix - this matrix (element-wise).
		MatrixCsr rsub(const MatrixCsr& m) const;

		//! Returns input scalar * this matrix.
		MatrixCsr rmul(const T& s) const;

		//! Returns input matrix / this scalar.
		MatrixCsr rdiv(const T& s) const;

		// MARK: Augmented operator methods - this instance (+)= input

		//! Adds input scalar to this matrix.
		void iadd(const T& s);

		//! Adds input matrix to this matrix (element-wise).
		void iadd(const MatrixCsr& m);

		//! Subtracts input scalar from this matrix.
		void isub(const T& s);

		//! Subtracts input matrix from this matrix (element-wise).
		void isub(const MatrixCsr& m);

		//! Multiplies input scalar to this matrix.
		void imul(const T& s);

		//! Multiplies input matrix to this matrix.
		template <typename ME>
		void imul(const MatrixExpression<T, ME>& m);

		//! Divides this matrix with input scalar.
		void idiv(const T& s);

		// MARK: Complex getters

		//! Returns sum of all elements.
		T sum() const;

		//! Returns average of all elements.
		T avg() const;

		//! Returns minimum among all elements.
		T min() const;

		//! Returns maximum among all elements.
		T max() const;

		//! Returns absolute minimum among all elements.
		T absmin() const;

		//! Returns absolute maximum among all elements.
		T absmax() const;

		//! Returns sum of all diagonal elements.
		//! \warning Should be a square matrix.
		T trace() const;

		//! Type-casts to different value-typed matrix.
		template <typename U>
		MatrixCsr<U> castTo() const;

		// MARK: Setter operators

		//!
		//! \brief Compresses input (dense) matrix expression into a sparse matrix.
		//!
		//! This function sets this sparse matrix with dense input matrix.
		//! During the process, zero elements (less than \p epsilon) will not be
		//! stored.
		//!
		template <typename E>
		MatrixCsr& operator=(const E& m);

		//! Copies to this matrix.
		MatrixCsr& operator=(const MatrixCsr& other);

		//! Moves to this matrix.
		MatrixCsr& operator=(MatrixCsr&& other);

		//! Addition assignment with input scalar.
		MatrixCsr& operator+=(const T& s);

		//! Addition assignment with input matrix (element-wise).
		MatrixCsr& operator+=(const MatrixCsr& m);

		//! Subtraction assignment with input scalar.
		MatrixCsr& operator-=(const T& s);

		//! Subtraction assignment with input matrix (element-wise).
		MatrixCsr& operator-=(const MatrixCsr& m);

		//! Multiplication assignment with input scalar.
		MatrixCsr& operator*=(const T& s);

		//! Multiplication assignment with input matrix.
		template <typename ME>
		MatrixCsr& operator*=(const MatrixExpression<T, ME>& m);

		//! Division assignment with input scalar.
		MatrixCsr& operator/=(const T& s);

		// MARK: Getter operators

		//! Returns (i,j) element.
		T operator()(size_t i, size_t j) const;

		//! Returns true if is equal to m.
		bool operator==(const MatrixCsr& m) const;

		//! Returns true if is not equal to m.
		bool operator!=(const MatrixCsr& m) const;

		// MARK: Builders
		//! Makes a m x m matrix with all diagonal elements to 1, and other elements
		//! to 0.
		static MatrixCsr<T> makeIdentity(size_t m);

	private:
		Size2 _size;
		NonZeroContainerType _nonZeros;
		IndexContainerType _rowPointers;
		IndexContainerType _columnIndices;

		size_t hasElement(size_t i, size_t j) const;

		template <typename Op>
		MatrixCsr binaryOp(const MatrixCsr& m, Op op) const;
	};

	//! Float-type CSR matrix.
	typedef MatrixCsr<float> MatrixCsrF;

	//! Double-type CSR matrix.
	typedef MatrixCsr<double> MatrixCsrD;

	//! -----------------------------Definition---------------------------------

	template <typename T, typename VE>
	MatrixCsrVectorMul<T, VE>::MatrixCsrVectorMul(const MatrixCsr<T>& m, const VE& v)
		: _m(m), _v(v) { assert(_m.cols() == _v.size()); }

	template <typename T, typename VE>
	MatrixCsrVectorMul<T, VE>::MatrixCsrVectorMul(const MatrixCsrVectorMul& other)
		: _m(other._m), _v(other._v) {}

	template <typename T, typename VE>
	size_t MatrixCsrVectorMul<T, VE>::size() const { return _m.rows(); }

	template <typename T, typename VE>
	T MatrixCsrVectorMul<T, VE>::operator[](size_t i) const 
	{
		auto rp = _m.rowPointersBegin();
		auto ci = _m.columnIndicesBegin();
		auto nnz = _m.nonZeroBegin();
		size_t colBegin = rp[i];
		size_t colEnd = rp[i + 1];
		T sum = 0;
		for (size_t jj = colBegin; jj < colEnd; ++jj) 
		{
			size_t j = ci[jj];
			sum += nnz[jj] * _v[j];
		}
		return sum;
	}

	//

	template <typename T, typename ME>
	MatrixCsrMatrixMul<T, ME>::MatrixCsrMatrixMul(const MatrixCsr<T>& m1, const ME& m2)
		: _m1(m1), _m2(m2), _nnz(m1.nonZeroData()), _rp(m1.rowPointersData()), _ci(m1.columnIndicesData()) {}

	template <typename T, typename ME>
	Size2 MatrixCsrMatrixMul<T, ME>::size() const { return { rows(), cols() }; }

	template <typename T, typename ME>
	size_t MatrixCsrMatrixMul<T, ME>::rows() const { return _m1.rows(); }

	template <typename T, typename ME>
	size_t MatrixCsrMatrixMul<T, ME>::cols() const { return _m2.cols(); }

	template <typename T, typename ME>
	T MatrixCsrMatrixMul<T, ME>::operator()(size_t i, size_t j) const 
	{
		size_t colBegin = _rp[i];
		size_t colEnd = _rp[i + 1];
		T sum = 0;
		for (size_t kk = colBegin; kk < colEnd; ++kk) 
		{
			size_t k = _ci[kk];
			sum += _nnz[kk] * _m2(k, j);
		}
		return sum;
	}

	//
	template <typename T>
	MatrixCsr<T>::Element::Element() : i(0), j(0), value(0) {}

	template <typename T>
	MatrixCsr<T>::Element::Element(size_t i_, size_t j_, const T& value_)
		: i(i_), j(j_), value(value_) {}

	//

	template <typename T>
	MatrixCsr<T>::MatrixCsr() { clear(); }

	template <typename T>
	MatrixCsr<T>::MatrixCsr(const std::initializer_list<std::initializer_list<T>>& lst, T epsilon) 
	{ compress(lst, epsilon); }

	template <typename T>
	template <typename E>
	MatrixCsr<T>::MatrixCsr(const MatrixExpression<T, E>& other, T epsilon) 
	{ compress(other, epsilon); }

	template <typename T>
	MatrixCsr<T>::MatrixCsr(const MatrixCsr& other) { set(other); }

	template <typename T>
	MatrixCsr<T>::MatrixCsr(MatrixCsr&& other) { (*this) = std::move(other); }

	template <typename T>
	void MatrixCsr<T>::clear() 
	{
		_size = { 0, 0 };
		_nonZeros.clear();
		_rowPointers.clear();
		_columnIndices.clear();
		_rowPointers.push_back(0);
	}

	template <typename T>
	void MatrixCsr<T>::set(const T& s) { std::fill(_nonZeros.begin(), _nonZeros.end(), s); }

	template <typename T>
	void MatrixCsr<T>::set(const MatrixCsr& other) 
	{
		_size = other._size;
		_nonZeros = other._nonZeros;
		_rowPointers = other._rowPointers;
		_columnIndices = other._columnIndices;
	}

	template <typename T>
	void MatrixCsr<T>::reserve(size_t rows, size_t cols, size_t numNonZeros) 
	{
		_size = Size2(rows, cols);
		_nonZeros.resize(numNonZeros);
		_rowPointers.resize(_size.x + 1);
		_columnIndices.resize(numNonZeros);
	}

	template <typename T>
	void MatrixCsr<T>::compress(const std::initializer_list<std::initializer_list<T>>& lst, T epsilon) 
	{
		size_t numRows = lst.size();
		size_t numCols = (numRows > 0) ? lst.begin()->size() : 0;
		_size = { numRows, numCols };
		_nonZeros.clear();
		_rowPointers.clear();
		_columnIndices.clear();
		auto rowIter = lst.begin();
		for (size_t i = 0; i < numRows; ++i) 
		{
			assert(numCols == rowIter->size());
			_rowPointers.push_back(_nonZeros.size());
			auto colIter = rowIter->begin();
			for (size_t j = 0; j < numCols; ++j) 
			{
				if (std::fabs(*colIter) > epsilon) 
				{
					_nonZeros.push_back(*colIter);
					_columnIndices.push_back(j);
				}
				++colIter;
			}
			++rowIter;
		}
		_rowPointers.push_back(_nonZeros.size());
	}

	template <typename T>
	template <typename E>
	void MatrixCsr<T>::compress(const MatrixExpression<T, E>& other, T epsilon) 
	{
		size_t numRows = other.rows();
		size_t numCols = other.cols();
		_size = { numRows, numCols };
		_nonZeros.clear();
		_columnIndices.clear();
		const E& expression = other();
		for (size_t i = 0; i < numRows; ++i) 
		{
			_rowPointers.push_back(_nonZeros.size());
			for (size_t j = 0; j < numCols; ++j) 
			{
				T val = expression(i, j);
				if (std::fabs(val) > epsilon) 
				{
					_nonZeros.push_back(val);
					_columnIndices.push_back(j);
				}
			}
		}
		_rowPointers.push_back(_nonZeros.size());
	}

	template <typename T>
	void MatrixCsr<T>::addElement(size_t i, size_t j, const T& value) 
	{ addElement({ i, j, value }); }

	template <typename T>
	void MatrixCsr<T>::addElement(const Element& element) 
	{
		ssize_t numRowsToAdd = (ssize_t)element.i - (ssize_t)_size.x + 1;
		if (numRowsToAdd > 0)
			for (ssize_t i = 0; i < numRowsToAdd; ++i)
				addRow({}, {});

		_size.y = std::max(_size.y, element.j + 1);

		size_t rowBegin = _rowPointers[element.i];
		size_t rowEnd = _rowPointers[element.i + 1];

		auto colIdxIter =
			std::lower_bound(_columnIndices.begin() + rowBegin,
				_columnIndices.begin() + rowEnd, element.j);
		auto offset = colIdxIter - _columnIndices.begin();

		_columnIndices.insert(colIdxIter, element.j);
		_nonZeros.insert(_nonZeros.begin() + offset, element.value);
		for (size_t i = element.i + 1; i < _rowPointers.size(); ++i) 
			++_rowPointers[i];
	}

	template <typename T>
	void MatrixCsr<T>::addRow(const NonZeroContainerType& nonZeros,
		const IndexContainerType& columnIndices) 
	{
		assert(nonZeros.size() == columnIndices.size());
		++_size.x;
		// TODO: Implement zip iterator
		std::vector<std::pair<T, size_t>> zipped;
		for (size_t i = 0; i < nonZeros.size(); ++i) 
		{
			zipped.emplace_back(nonZeros[i], columnIndices[i]);
			_size.y = std::max(_size.y, columnIndices[i] + 1);
		}
		// sort by column index.
		std::sort(zipped.begin(), zipped.end(),
			[](std::pair<T, size_t> a, std::pair<T, size_t> b) {
			return a.second < b.second; });
		for (size_t i = 0; i < zipped.size(); ++i) 
		{
			_nonZeros.push_back(zipped[i].first);
			_columnIndices.push_back(zipped[i].second);
		}
		_rowPointers.push_back(_nonZeros.size());
	}

	template <typename T>
	void MatrixCsr<T>::setElement(size_t i, size_t j, const T& value) 
	{ setElement({ i, j, value }); }

	template <typename T>
	void MatrixCsr<T>::setElement(const Element& element) 
	{
		size_t nzIndex = hasElement(element.i, element.j);
		if (nzIndex == kMaxSize) 
			addElement(element);
		else 
			_nonZeros[nzIndex] = element.value;
	}

	template <typename T>
	bool MatrixCsr<T>::isEqual(const MatrixCsr& other) const 
	{
		if (_size != other._size) 
			return false;
		if (_nonZeros.size() != other._nonZeros.size()) 
			return false;
		for (size_t i = 0; i < _nonZeros.size(); ++i) 
		{
			if (_nonZeros[i] != other._nonZeros[i]) 
				return false;
			if (_columnIndices[i] != other._columnIndices[i]) 
				return false;
		}
		for (size_t i = 0; i < _rowPointers.size(); ++i) 
			if (_rowPointers[i] != other._rowPointers[i]) 
				return false;
		return true;
	}

	template <typename T>
	bool MatrixCsr<T>::isSimilar(const MatrixCsr& other, double tol) const 
	{
		if (_size != other._size) 
			return false;
		if (_nonZeros.size() != other._nonZeros.size()) 
			return false;
		for (size_t i = 0; i < _nonZeros.size(); ++i) 
		{
			if (std::fabs(_nonZeros[i] - other._nonZeros[i]) > tol) 
				return false;
			if (_columnIndices[i] != other._columnIndices[i]) 
				return false;
		}
		for (size_t i = 0; i < _rowPointers.size(); ++i) 
			if (_rowPointers[i] != other._rowPointers[i]) 
				return false;		
		return true;
	}

	template <typename T>
	bool MatrixCsr<T>::isSquare() const { return rows() == cols(); }

	template <typename T>
	Size2 MatrixCsr<T>::size() const { return _size; }

	template <typename T>
	size_t MatrixCsr<T>::rows() const { return _size.x; }

	template <typename T>
	size_t MatrixCsr<T>::cols() const { return _size.y; }

	template <typename T>
	size_t MatrixCsr<T>::numberOfNonZeros() const { return _nonZeros.size(); }

	template <typename T>
	const T& MatrixCsr<T>::nonZero(size_t i) const { return _nonZeros[i]; }

	template <typename T>
	T& MatrixCsr<T>::nonZero(size_t i) { return _nonZeros[i]; }

	template <typename T>
	const size_t& MatrixCsr<T>::rowPointer(size_t i) const { return _rowPointers[i]; }

	template <typename T>
	const size_t& MatrixCsr<T>::columnIndex(size_t i) const { return _columnIndices[i]; }

	template <typename T>
	T* MatrixCsr<T>::nonZeroData() { return _nonZeros.data(); }

	template <typename T>
	const T* const MatrixCsr<T>::nonZeroData() const { return _nonZeros.data(); }

	template <typename T>
	const size_t* const MatrixCsr<T>::rowPointersData() const { return _rowPointers.data(); }

	template <typename T>
	const size_t* const MatrixCsr<T>::columnIndicesData() const { return _columnIndices.data(); }

	template <typename T>
	typename MatrixCsr<T>::NonZeroIterator MatrixCsr<T>::nonZeroBegin() { return _nonZeros.begin(); }

	template <typename T>
	typename MatrixCsr<T>::ConstNonZeroIterator MatrixCsr<T>::nonZeroBegin() const { return _nonZeros.cbegin(); }

	template <typename T>
	typename MatrixCsr<T>::NonZeroIterator MatrixCsr<T>::nonZeroEnd() { return _nonZeros.end(); }

	template <typename T>
	typename MatrixCsr<T>::ConstNonZeroIterator MatrixCsr<T>::nonZeroEnd() const { return _nonZeros.cend(); }

	template <typename T>
	typename MatrixCsr<T>::IndexIterator MatrixCsr<T>::rowPointersBegin() { return _rowPointers.begin(); }

	template <typename T>
	typename MatrixCsr<T>::ConstIndexIterator MatrixCsr<T>::rowPointersBegin() const 
	{ return _rowPointers.cbegin(); }

	template <typename T>
	typename MatrixCsr<T>::IndexIterator MatrixCsr<T>::rowPointersEnd() 
	{ return _rowPointers.end(); }

	template <typename T>
	typename MatrixCsr<T>::ConstIndexIterator MatrixCsr<T>::rowPointersEnd() const 
	{ return _rowPointers.cend(); }

	template <typename T>
	typename MatrixCsr<T>::IndexIterator MatrixCsr<T>::columnIndicesBegin() 
	{ return _columnIndices.begin(); }

	template <typename T>
	typename MatrixCsr<T>::ConstIndexIterator MatrixCsr<T>::columnIndicesBegin() const 
	{ return _columnIndices.cbegin(); }

	template <typename T>
	typename MatrixCsr<T>::IndexIterator MatrixCsr<T>::columnIndicesEnd() 
	{ return _columnIndices.end(); }

	template <typename T>
	typename MatrixCsr<T>::ConstIndexIterator MatrixCsr<T>::columnIndicesEnd() const 
	{ return _columnIndices.cend(); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::add(const T& s) const 
	{
		MatrixCsr ret(*this);
		parallelFor(kZeroSize, ret._nonZeros.size(),
			[&](size_t i) { ret._nonZeros[i] += s; });
		return ret;
	}

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::add(const MatrixCsr& m) const 
	{ return binaryOp(m, std::plus<T>()); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::sub(const T& s) const 
	{
		MatrixCsr ret(*this);
		parallelFor(kZeroSize, ret._nonZeros.size(),
			[&](size_t i) { ret._nonZeros[i] -= s; });
		return ret;
	}

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::sub(const MatrixCsr& m) const 
	{ return binaryOp(m, std::minus<T>()); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::mul(const T& s) const 
	{
		MatrixCsr ret(*this);
		parallelFor(kZeroSize, ret._nonZeros.size(),
			[&](size_t i) { ret._nonZeros[i] *= s; });
		return ret;
	}

	template <typename T>
	template <typename VE>
	MatrixCsrVectorMul<T, VE> MatrixCsr<T>::mul(const VectorExpression<T, VE>& v) const 
	{ return MatrixCsrVectorMul<T, VE>(*this, v()); };

	template <typename T>
	template <typename ME>
	MatrixCsrMatrixMul<T, ME> MatrixCsr<T>::mul(const MatrixExpression<T, ME>& m) const 
	{ return MatrixCsrMatrixMul<T, ME>(*this, m()); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::div(const T& s) const 
	{
		MatrixCsr ret(*this);
		parallelFor(kZeroSize, ret._nonZeros.size(),
			[&](size_t i) { ret._nonZeros[i] /= s; });
		return ret;
	}

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::radd(const T& s) const { return add(s); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::radd(const MatrixCsr& m) const { return add(m); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::rsub(const T& s) const 
	{
		MatrixCsr ret(*this);
		parallelFor(kZeroSize, ret._nonZeros.size(),
			[&](size_t i) { ret._nonZeros[i] = s - ret._nonZeros[i]; });
		return ret;
	}

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::rsub(const MatrixCsr& m) const { return m.sub(*this); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::rmul(const T& s) const { return mul(s); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::rdiv(const T& s) const 
	{
		MatrixCsr ret(*this);
		parallelFor(kZeroSize, ret._nonZeros.size(),
			[&](size_t i) { ret._nonZeros[i] = s / ret._nonZeros[i]; });
		return ret;
	}

	template <typename T>
	void MatrixCsr<T>::iadd(const T& s) 
	{
		parallelFor(kZeroSize, _nonZeros.size(), [&](size_t i) { _nonZeros[i] += s; });
	}

	template <typename T>
	void MatrixCsr<T>::iadd(const MatrixCsr& m) { set(add(m)); }

	template <typename T>
	void MatrixCsr<T>::isub(const T& s) 
	{
		parallelFor(kZeroSize, _nonZeros.size(), [&](size_t i) { _nonZeros[i] -= s; });
	}

	template <typename T>
	void MatrixCsr<T>::isub(const MatrixCsr& m) { set(sub(m)); }

	template <typename T>
	void MatrixCsr<T>::imul(const T& s) 
	{
		parallelFor(kZeroSize, _nonZeros.size(), [&](size_t i) { _nonZeros[i] *= s; });
	}

	template <typename T>
	template <typename ME>
	void MatrixCsr<T>::imul(const MatrixExpression<T, ME>& m) 
	{
		MatrixCsrD result = mul(m);
		*this = std::move(result);
	}

	template <typename T>
	void MatrixCsr<T>::idiv(const T& s) 
	{
		parallelFor(kZeroSize, _nonZeros.size(), [&](size_t i) { _nonZeros[i] /= s; });
	}

	template <typename T>
	T MatrixCsr<T>::sum() const 
	{
		return parallelReduce(kZeroSize, numberOfNonZeros(), T(0),
			[&](size_t start, size_t end, T init) {
			T result = init;
			for (size_t i = start; i < end; ++i) 
				result += _nonZeros[i];
			return result; }, std::plus<T>());
	}

	template <typename T>
	T MatrixCsr<T>::avg() const { return sum() / numberOfNonZeros(); }

	template <typename T>
	T MatrixCsr<T>::min() const 
	{
		const T& (*_min)(const T&, const T&) = std::min<T>;
		return parallelReduce(kZeroSize, numberOfNonZeros(),
			std::numeric_limits<T>::max(),
			[&](size_t start, size_t end, T init) {
			T result = init;
			for (size_t i = start; i < end; ++i) 
				result = std::min(result, _nonZeros[i]);
			return result; }, _min);
	}

	template <typename T>
	T MatrixCsr<T>::max() const 
	{
		const T& (*_max)(const T&, const T&) = std::max<T>;
		return parallelReduce(kZeroSize, numberOfNonZeros(),
			std::numeric_limits<T>::min(),
			[&](size_t start, size_t end, T init) {
			T result = init;
			for (size_t i = start; i < end; ++i) 
				result = std::max(result, _nonZeros[i]);
			return result; }, _max);
	}

	template <typename T>
	T MatrixCsr<T>::absmin() const 
	{
		return parallelReduce(kZeroSize, numberOfNonZeros(),
			std::numeric_limits<T>::max(),
			[&](size_t start, size_t end, T init) {
			T result = init;
			for (size_t i = start; i < end; ++i) 
				result = Engine::absmin(result, _nonZeros[i]);
			return result; }, Engine::absmin<T>);
	}

	template <typename T>
	T MatrixCsr<T>::absmax() const 
	{
		return parallelReduce(kZeroSize, numberOfNonZeros(), T(0),
			[&](size_t start, size_t end, T init) {
			T result = init;
			for (size_t i = start; i < end; ++i) 
				result = Engine::absmax(result, _nonZeros[i]);
			return result; }, Engine::absmax<T>);
	}

	template <typename T>
	T MatrixCsr<T>::trace() const 
	{
		assert(isSquare());
		return parallelReduce(kZeroSize, rows(), T(0),
			[&](size_t start, size_t end, T init) {
			T result = init;
			for (size_t i = start; i < end; ++i) 
				result += (*this)(i, i);
			return result; }, std::plus<T>());
	}

	template <typename T>
	template <typename U>
	MatrixCsr<U> MatrixCsr<T>::castTo() const 
	{
		MatrixCsr<U> ret;
		ret.reserve(rows(), cols(), numberOfNonZeros());
		auto nnz = ret.nonZeroBegin();
		auto ci = ret.columnIndicesBegin();
		auto rp = ret.rowPointersBegin();
		parallelFor(kZeroSize, _nonZeros.size(), [&](size_t i) {
			nnz[i] = static_cast<U>(_nonZeros[i]);
			ci[i] = _columnIndices[i]; });
		parallelFor(kZeroSize, _rowPointers.size(),
			[&](size_t i) { rp[i] = _rowPointers[i]; });
		return ret;
	}

	template <typename T>
	template <typename E>
	MatrixCsr<T>& MatrixCsr<T>::operator=(const E& m) { set(m); return *this; }

	template <typename T>
	MatrixCsr<T>& MatrixCsr<T>::operator=(const MatrixCsr& other) { set(other); return *this; }

	template <typename T>
	MatrixCsr<T>& MatrixCsr<T>::operator=(MatrixCsr&& other) 
	{
		_size = other._size;
		other._size = Size2();
		_nonZeros = std::move(other._nonZeros);
		_rowPointers = std::move(other._rowPointers);
		_columnIndices = std::move(other._columnIndices);
		return *this;
	}

	template <typename T>
	MatrixCsr<T>& MatrixCsr<T>::operator+=(const T& s) { iadd(s); return *this; }

	template <typename T>
	MatrixCsr<T>& MatrixCsr<T>::operator+=(const MatrixCsr& m) { iadd(m); return *this; }

	template <typename T>
	MatrixCsr<T>& MatrixCsr<T>::operator-=(const T& s) { isub(s); return *this; }

	template <typename T>
	MatrixCsr<T>& MatrixCsr<T>::operator-=(const MatrixCsr& m) { isub(m); return *this; }

	template <typename T>
	MatrixCsr<T>& MatrixCsr<T>::operator*=(const T& s) { imul(s); return *this; }

	template <typename T>
	template <typename ME>
	MatrixCsr<T>& MatrixCsr<T>::operator*=(const MatrixExpression<T, ME>& m) { imul(m); return *this; }

	template <typename T>
	MatrixCsr<T>& MatrixCsr<T>::operator/=(const T& s) { idiv(s); return *this; }

	template <typename T>
	T MatrixCsr<T>::operator()(size_t i, size_t j) const 
	{
		size_t nzIndex = hasElement(i, j);
		if (nzIndex == kMaxSize) 
			return 0.0;
		else 
			return _nonZeros[nzIndex];
	}

	template <typename T>
	bool MatrixCsr<T>::operator==(const MatrixCsr& m) const { return isEqual(m); }

	template <typename T>
	bool MatrixCsr<T>::operator!=(const MatrixCsr& m) const { return !isEqual(m); }

	template <typename T>
	MatrixCsr<T> MatrixCsr<T>::makeIdentity(size_t m) 
	{
		MatrixCsr ret;
		ret._size = Size2(m, m);
		ret._nonZeros.resize(m, 1.0);
		ret._columnIndices.resize(m);
		std::iota(ret._columnIndices.begin(), ret._columnIndices.end(), kZeroSize);
		ret._rowPointers.resize(m + 1);
		std::iota(ret._rowPointers.begin(), ret._rowPointers.end(), kZeroSize);
		return ret;
	}

	template <typename T>
	size_t MatrixCsr<T>::hasElement(size_t i, size_t j) const 
	{
		if (i >= _size.x || j >= _size.y) 
			return kMaxSize;
		size_t rowBegin = _rowPointers[i];
		size_t rowEnd = _rowPointers[i + 1];
		auto iter = binaryFind(_columnIndices.begin() + rowBegin,
			_columnIndices.begin() + rowEnd, j);
		if (iter != _columnIndices.begin() + rowEnd) 
			return static_cast<size_t>(iter - _columnIndices.begin());
		else 
			return kMaxSize;
	}

	template <typename T>
	template <typename Op>
	MatrixCsr<T> MatrixCsr<T>::binaryOp(const MatrixCsr& m, Op op) const 
	{
		assert(_size == m._size);
		MatrixCsr ret;
		for (size_t i = 0; i < _size.x; ++i) 
		{
			std::vector<size_t> col;
			std::vector<double> nnz;
			auto colIterA = _columnIndices.begin() + _rowPointers[i];
			auto colIterB = m._columnIndices.begin() + m._rowPointers[i];
			auto colEndA = _columnIndices.begin() + _rowPointers[i + 1];
			auto colEndB = m._columnIndices.begin() + m._rowPointers[i + 1];
			auto nnzIterA = _nonZeros.begin() + _rowPointers[i];
			auto nnzIterB = m._nonZeros.begin() + m._rowPointers[i];
			while (colIterA != colEndA || colIterB != colEndB) 
			{
				if (colIterB == colEndB || *colIterA < *colIterB) 
				{
					col.push_back(*colIterA);
					nnz.push_back(op(*nnzIterA, 0));
					++colIterA;
					++nnzIterA;
				}
				else if (colIterA == colEndA || *colIterA > *colIterB) 
				{
					col.push_back(*colIterB);
					nnz.push_back(op(0, *nnzIterB));
					++colIterB;
					++nnzIterB;
				}
				else 
				{
					assert(*colIterA == *colIterB);
					col.push_back(*colIterB);
					nnz.push_back(op(*nnzIterA, *nnzIterB));
					++colIterA;
					++nnzIterA;
					++colIterB;
					++nnzIterB;
				}
			}
			ret.addRow(nnz, col);
		}
		return ret;
	}

	// MARK: Operator overloadings

	template <typename T>
	MatrixCsr<T> operator-(const MatrixCsr<T>& a) { return a.mul(-1); }

	template <typename T>
	MatrixCsr<T> operator+(const MatrixCsr<T>& a, const MatrixCsr<T>& b) { return a.add(b); }

	template <typename T>
	MatrixCsr<T> operator+(const MatrixCsr<T>& a, T b) { return a.add(b); }

	template <typename T>
	MatrixCsr<T> operator+(T a, const MatrixCsr<T>& b) { return b.add(a); 	}

	template <typename T>
	MatrixCsr<T> operator-(const MatrixCsr<T>& a, const MatrixCsr<T>& b) { return a.sub(b); }

	template <typename T>
	MatrixCsr<T> operator-(const MatrixCsr<T>& a, T b) { return a.sub(b); }

	template <typename T>
	MatrixCsr<T> operator-(T a, const MatrixCsr<T>& b) { return b.rsub(a); }

	template <typename T>
	MatrixCsr<T> operator*(const MatrixCsr<T>& a, T b) { return a.mul(b); }

	template <typename T>
	MatrixCsr<T> operator*(T a, const MatrixCsr<T>& b) { return b.rmul(a); }

	template <typename T, typename VE>
	MatrixCsrVectorMul<T, VE> operator*(const MatrixCsr<T>& a, const VectorExpression<T, VE>& b) 
	{ return a.mul(b); }

	template <typename T, typename ME>
	MatrixCsrMatrixMul<T, ME> operator*(const MatrixCsr<T>& a, const MatrixExpression<T, ME>& b) 
	{ return a.mul(b); }

	template <typename T>
	MatrixCsr<T> operator/(const MatrixCsr<T>& a, T b) { return a.div(b); }

	template <typename T>
	MatrixCsr<T> operator/(T a, const MatrixCsr<T>& b) { return b.rdiv(a); }

}