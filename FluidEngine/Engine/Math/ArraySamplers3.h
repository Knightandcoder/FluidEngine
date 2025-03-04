#pragma once

#include <limits>
#include <algorithm>
#include <functional>

#include "Vector3.h"
#include "ArraySamplers.h"
#include "ArrayAccessor3.h"
#include "../Utils/Macros.h"
#include "../Utils/MathUtils.h"

namespace Engine
{
	//!
	//! \brief 3-D nearest array sampler class.
	//!
	//! This class provides nearest sampling interface for a given 3-D array.
	//!
	//! \tparam T - The value type to sample.
	//! \tparam R - The real number type.
	//!
	template <typename T, typename R>
	class NearestArraySampler<T, R, 3> final
	{
	public:

		static_assert(std::is_floating_point<R>::value, "Samplers only can be instantiated with floating point types");

		//!
		//! \brief      Constructs a sampler using array accessor, spacing between
		//!     the elements, and the position of the first array element.
		//!
		//! \param[in]  accessor    The array accessor.
		//! \param[in]  gridSpacing The grid spacing.
		//! \param[in]  gridOrigin  The grid origin.
		//!
		explicit NearestArraySampler(const ConstArrayAccessor3<T>& accessor, const Vector3<R>& gridSpacing,
			const Vector3<R>& gridOrigin);

		//! Copy constructor.
		NearestArraySampler(const NearestArraySampler& other);

		//! Returns sampled value at point \p pt.
		T operator()(const Vector3<R>& pt) const;

		//! Returns the nearest array index for point \p x.
		void getCoordinate(const Vector3<R>& pt, Point3UI* index) const;

		//! Returns a funtion object that wraps this instance.
		std::function<T(const Vector3<R>&)> functor() const;

	private:
		Vector3<R> _gridSpacing;
		Vector3<R> _origin;
		ConstArrayAccessor3<T> _accessor;

	};

	//! Type alias for 3-D nearest array sampler.
	template <typename T, typename R> using NearestArraySampler3 = NearestArraySampler<T, R, 3>;

	//!
	//! \brief 2-D linear array sampler class.
	//!
	//! This class provides linear sampling interface for a given 3-D array.
	//!
	//! \tparam T - The value type to sample.
	//! \tparam R - The real number type.
	//!
	template <typename T, typename R>
	class LinearArraySampler<T, R, 3> final
	{
	public:

		static_assert(std::is_floating_point<R>::value, "Samplers only can be instantiated with floating point types");

		//!
		//! \brief      Constructs a sampler using array accessor, spacing between
		//!     the elements, and the position of the first array element.
		//!
		//! \param[in]  accessor    The array accessor.
		//! \param[in]  gridSpacing The grid spacing.
		//! \param[in]  gridOrigin  The grid origin.
		//!
		explicit LinearArraySampler(const ConstArrayAccessor3<T>& accessor, const Vector3<R>& gridSpacing,
			const Vector3<R>& gridOrigin);

		//! Copy constructor.
		LinearArraySampler(const LinearArraySampler& other);

		//! Returns sampled value at point \p pt.
		T operator()(const Vector3<R>& pt) const;

		//! Returns the indices of points and their sampling weight for given point.
		void getCoordinatesAndWeights(const Vector3<R>& pt, std::array<Point3UI, 8>* indices,
			std::array<R, 8>* weights) const;

		//! Returns the indices of points and their gradient of sampling weight for
		//! given point.
		void getCoordinatesAndGradientWeights(const Vector3<R>& pt, std::array<Point3UI, 8>* indices,
			std::array<Vector3<R>, 8>* weights) const;

		//! Returns a funtion object that wraps this instance.
		std::function<T(const Vector3<R>&)> functor() const;

	private:
		Vector3<R> _gridSpacing;
		Vector3<R> _invGridSpacing;
		Vector3<R> _origin;
		ConstArrayAccessor3<T> _accessor;

	};

	//! Type alias for 3-D linear array sampler.
	template <typename T, typename R> using LinearArraySampler3 = LinearArraySampler<T, R, 3>;

	//!
	//! \brief 3-D cubic array sampler class.
	//!
	//! This class provides cubic sampling interface for a given 3-D array.
	//!
	//! \tparam T - The value type to sample.
	//! \tparam R - The real number type.
	//!
	template <typename T, typename R>
	class CubicArraySampler<T, R, 3> final
	{
	public:

		static_assert(std::is_floating_point<R>::value, "Samplers only can be instantiated with floating point types");

		//!
		//! \brief      Constructs a sampler using array accessor, spacing between
		//!     the elements, and the position of the first array element.
		//!
		//! \param[in]  accessor    The array accessor.
		//! \param[in]  gridSpacing The grid spacing.
		//! \param[in]  gridOrigin  The grid origin.
		//!
		explicit CubicArraySampler(const ConstArrayAccessor3<T>& accessor, const Vector3<R>& gridSpacing,
			const Vector3<R>& gridOrigin);

		//! Copy constructor.
		CubicArraySampler(const CubicArraySampler& other);

		//! Returns sampled value at point \p pt.
		T operator()(const Vector3<R>& pt) const;

		//! Returns a funtion object that wraps this instance.
		std::function<T(const Vector3<R>&)> functor() const;

	private:
		Vector3<R> _gridSpacing;
		Vector3<R> _origin;
		ConstArrayAccessor3<T> _accessor;

	};

	//! Type alias for 3-D cubic array sampler.
	template <typename T, typename R> using CubicArraySampler3 = CubicArraySampler<T, R, 3>;

	//! -------------------------------------Definition-----------------------------------------------------

	template <typename T, typename R>
	NearestArraySampler<T, R, 3>::NearestArraySampler(const ConstArrayAccessor3<T>& accessor,
		const Vector3<R>& gridSpacing, const Vector3<R>& gridOrigin)
	{
		_gridSpacing = gridSpacing;
		_origin = gridOrigin;
		_accessor = accessor;
	}

	template <typename T, typename R>
	NearestArraySampler<T, R, 3>::NearestArraySampler(const NearestArraySampler& other) 
	{
		_gridSpacing = other._gridSpacing;
		_origin = other._origin;
		_accessor = other._accessor;
	}

	template <typename T, typename R>
	T NearestArraySampler<T, R, 3>::operator()(const Vector3<R>& x) const 
	{
		ssize_t i, j, k;
		R fx, fy, fz;

		assert(_gridSpacing.x > std::numeric_limits<R>::epsilon() &&
			_gridSpacing.y > std::numeric_limits<R>::epsilon() &&
			_gridSpacing.z > std::numeric_limits<R>::epsilon());
		Vector3<R> normalizedX = (x - _origin) / _gridSpacing;

		ssize_t iSize = static_cast<ssize_t>(_accessor.size().x);
		ssize_t jSize = static_cast<ssize_t>(_accessor.size().y);
		ssize_t kSize = static_cast<ssize_t>(_accessor.size().z);

		getBarycentric(normalizedX.x, 0, iSize - 1, &i, &fx);
		getBarycentric(normalizedX.y, 0, jSize - 1, &j, &fy);
		getBarycentric(normalizedX.z, 0, kSize - 1, &k, &fz);

		i = std::min(static_cast<ssize_t>(i + fx + 0.5), iSize - 1);
		j = std::min(static_cast<ssize_t>(j + fy + 0.5), jSize - 1);
		k = std::min(static_cast<ssize_t>(k + fz + 0.5), kSize - 1);

		return _accessor(i, j, k);
	}

	template <typename T, typename R>
	void NearestArraySampler<T, R, 3>::getCoordinate(const Vector3<R>& x, Point3UI* index) const
	{
		ssize_t i, j, k;
		R fx, fy, fz;

		assert(_gridSpacing.x > std::numeric_limits<R>::epsilon() &&
			_gridSpacing.y > std::numeric_limits<R>::epsilon() &&
			_gridSpacing.z > std::numeric_limits<R>::epsilon());
		Vector3<R> normalizedX = (x - _origin) / _gridSpacing;

		ssize_t iSize = static_cast<ssize_t>(_accessor.size().x);
		ssize_t jSize = static_cast<ssize_t>(_accessor.size().y);
		ssize_t kSize = static_cast<ssize_t>(_accessor.size().z);

		getBarycentric(normalizedX.x, 0, iSize - 1, &i, &fx);
		getBarycentric(normalizedX.y, 0, jSize - 1, &j, &fy);
		getBarycentric(normalizedX.z, 0, kSize - 1, &k, &fz);

		index->x = std::min(static_cast<ssize_t>(i + fx + 0.5), iSize - 1);
		index->y = std::min(static_cast<ssize_t>(j + fy + 0.5), jSize - 1);
		index->z = std::min(static_cast<ssize_t>(k + fz + 0.5), kSize - 1);
	}

	template <typename T, typename R>
	std::function<T(const Vector3<R>&)> NearestArraySampler<T, R, 3>::functor() const 
	{
		NearestArraySampler sampler(*this);
		return std::bind(&NearestArraySampler::operator(), sampler, std::placeholders::_1);
	}

	template <typename T, typename R>
	LinearArraySampler<T, R, 3>::LinearArraySampler(const ConstArrayAccessor3<T>& accessor,
		const Vector3<R>& gridSpacing, const Vector3<R>& gridOrigin) 
	{
		_gridSpacing = gridSpacing;
		_invGridSpacing = static_cast<R>(1) / _gridSpacing;
		_origin = gridOrigin;
		_accessor = accessor;
	}

	template <typename T, typename R>
	LinearArraySampler<T, R, 3>::LinearArraySampler(const LinearArraySampler& other)
	{
		_gridSpacing = other._gridSpacing;
		_invGridSpacing = other._invGridSpacing;
		_origin = other._origin;
		_accessor = other._accessor;
	}

	template <typename T, typename R>
	T LinearArraySampler<T, R, 3>::operator()(const Vector3<R>& x) const 
	{
		ssize_t i, j, k;
		R fx, fy, fz;

		assert(_gridSpacing.x > std::numeric_limits<R>::epsilon() &&
			_gridSpacing.y > std::numeric_limits<R>::epsilon() &&
			_gridSpacing.z > std::numeric_limits<R>::epsilon());
		Vector3<R> normalizedX = (x - _origin) / _gridSpacing;

		ssize_t iSize = static_cast<ssize_t>(_accessor.size().x);
		ssize_t jSize = static_cast<ssize_t>(_accessor.size().y);
		ssize_t kSize = static_cast<ssize_t>(_accessor.size().z);

		getBarycentric(normalizedX.x, 0, iSize - 1, &i, &fx);
		getBarycentric(normalizedX.y, 0, jSize - 1, &j, &fy);
		getBarycentric(normalizedX.z, 0, kSize - 1, &k, &fz);

		ssize_t ip1 = std::min(i + 1, iSize - 1);
		ssize_t jp1 = std::min(j + 1, jSize - 1);
		ssize_t kp1 = std::min(k + 1, kSize - 1);

		return trilerp(_accessor(i, j, k), _accessor(ip1, j, k), _accessor(i, jp1, k), _accessor(ip1, jp1, k),
			_accessor(i, j, kp1), _accessor(ip1, j, kp1), _accessor(i, jp1, kp1), _accessor(ip1, jp1, kp1),
			fx, fy, fz);
	}

	template <typename T, typename R>
	void LinearArraySampler<T, R, 3>::getCoordinatesAndWeights(const Vector3<R>& x,
		std::array<Point3UI, 8>* indices, std::array<R, 8>* weights) const 
	{
		ssize_t i, j, k;
		R fx, fy, fz;

		assert(_gridSpacing.x > 0.0 && _gridSpacing.y > 0.0 && _gridSpacing.z > 0.0);

		const Vector3<R> normalizedX = (x - _origin) * _invGridSpacing;

		const ssize_t iSize = static_cast<ssize_t>(_accessor.size().x);
		const ssize_t jSize = static_cast<ssize_t>(_accessor.size().y);
		const ssize_t kSize = static_cast<ssize_t>(_accessor.size().z);

		getBarycentric(normalizedX.x, 0, iSize - 1, &i, &fx);
		getBarycentric(normalizedX.y, 0, jSize - 1, &j, &fy);
		getBarycentric(normalizedX.z, 0, kSize - 1, &k, &fz);

		const ssize_t ip1 = std::min(i + 1, iSize - 1);
		const ssize_t jp1 = std::min(j + 1, jSize - 1);
		const ssize_t kp1 = std::min(k + 1, kSize - 1);

		(*indices)[0] = Point3UI(i, j, k);
		(*indices)[1] = Point3UI(ip1, j, k);
		(*indices)[2] = Point3UI(i, jp1, k);
		(*indices)[3] = Point3UI(ip1, jp1, k);
		(*indices)[4] = Point3UI(i, j, kp1);
		(*indices)[5] = Point3UI(ip1, j, kp1);
		(*indices)[6] = Point3UI(i, jp1, kp1);
		(*indices)[7] = Point3UI(ip1, jp1, kp1);

		(*weights)[0] = (1 - fx) * (1 - fy) * (1 - fz);
		(*weights)[1] = fx * (1 - fy) * (1 - fz);
		(*weights)[2] = (1 - fx) * fy * (1 - fz);
		(*weights)[3] = fx * fy * (1 - fz);
		(*weights)[4] = (1 - fx) * (1 - fy) * fz;
		(*weights)[5] = fx * (1 - fy) * fz;
		(*weights)[6] = (1 - fx) * fy * fz;
		(*weights)[7] = fx * fy * fz;
	}

	template <typename T, typename R>
	void LinearArraySampler<T, R, 3>::getCoordinatesAndGradientWeights(const Vector3<R>& x,
		std::array<Point3UI, 8>* indices, std::array<Vector3<R>, 8>* weights) const 
	{
		ssize_t i, j, k;
		R fx, fy, fz;

		assert(_gridSpacing.x > 0.0 && _gridSpacing.y > 0.0 && _gridSpacing.z > 0.0);

		Vector3<R> normalizedX = (x - _origin) / _gridSpacing;

		ssize_t iSize = static_cast<ssize_t>(_accessor.size().x);
		ssize_t jSize = static_cast<ssize_t>(_accessor.size().y);
		ssize_t kSize = static_cast<ssize_t>(_accessor.size().z);

		getBarycentric(normalizedX.x, 0, iSize - 1, &i, &fx);
		getBarycentric(normalizedX.y, 0, jSize - 1, &j, &fy);
		getBarycentric(normalizedX.z, 0, kSize - 1, &k, &fz);

		ssize_t ip1 = std::min(i + 1, iSize - 1);
		ssize_t jp1 = std::min(j + 1, jSize - 1);
		ssize_t kp1 = std::min(k + 1, kSize - 1);

		(*indices)[0] = Point3UI(i, j, k);
		(*indices)[1] = Point3UI(ip1, j, k);
		(*indices)[2] = Point3UI(i, jp1, k);
		(*indices)[3] = Point3UI(ip1, jp1, k);
		(*indices)[4] = Point3UI(i, j, kp1);
		(*indices)[5] = Point3UI(ip1, j, kp1);
		(*indices)[6] = Point3UI(i, jp1, kp1);
		(*indices)[7] = Point3UI(ip1, jp1, kp1);

		(*weights)[0] = Vector3<R>(
			-_invGridSpacing.x * (1 - fy) * (1 - fz),
			-_invGridSpacing.y * (1 - fx) * (1 - fz),
			-_invGridSpacing.z * (1 - fx) * (1 - fy));
		(*weights)[1] = Vector3<R>(
			_invGridSpacing.x * (1 - fy) * (1 - fz),
			fx * (-_invGridSpacing.y) * (1 - fz),
			fx * (1 - fy) * (-_invGridSpacing.z));
		(*weights)[2] = Vector3<R>(
			(-_invGridSpacing.x) * fy * (1 - fz),
			(1 - fx) * _invGridSpacing.y * (1 - fz),
			(1 - fx) * fy * (-_invGridSpacing.z));
		(*weights)[3] = Vector3<R>(
			_invGridSpacing.x * fy * (1 - fz),
			fx * _invGridSpacing.y * (1 - fz),
			fx * fy * (-_invGridSpacing.z));
		(*weights)[4] = Vector3<R>(
			(-_invGridSpacing.x) * (1 - fy) * fz,
			(1 - fx) * (-_invGridSpacing.y) * fz,
			(1 - fx) * (1 - fy) * _invGridSpacing.z);
		(*weights)[5] = Vector3<R>(
			_invGridSpacing.x * (1 - fy) * fz,
			fx * (-_invGridSpacing.y) * fz,
			fx * (1 - fy) * _invGridSpacing.z);
		(*weights)[6] = Vector3<R>(
			(-_invGridSpacing.x) * fy * fz,
			(1 - fx) * _invGridSpacing.y * fz,
			(1 - fx) * fy * _invGridSpacing.z);
		(*weights)[7] = Vector3<R>(
			_invGridSpacing.x * fy * fz,
			fx * _invGridSpacing.y * fz,
			fx * fy * _invGridSpacing.z);
	}

	template <typename T, typename R>
	std::function<T(const Vector3<R>&)> LinearArraySampler<T, R, 3>::functor() const 
	{
		LinearArraySampler sampler(*this);
		return std::bind(&LinearArraySampler::operator(), sampler, std::placeholders::_1);
	}

	template <typename T, typename R>
	CubicArraySampler<T, R, 3>::CubicArraySampler(const ConstArrayAccessor3<T>& accessor,
		const Vector3<R>& gridSpacing, const Vector3<R>& gridOrigin) 
	{
		_gridSpacing = gridSpacing;
		_origin = gridOrigin;
		_accessor = accessor;
	}

	template <typename T, typename R>
	CubicArraySampler<T, R, 3>::CubicArraySampler(const CubicArraySampler& other) 
	{
		_gridSpacing = other._gridSpacing;
		_origin = other._origin;
		_accessor = other._accessor;
	}

	template <typename T, typename R>
	T CubicArraySampler<T, R, 3>::operator()(const Vector3<R>& x) const 
	{
		ssize_t i, j, k;
		ssize_t iSize = static_cast<ssize_t>(_accessor.size().x);
		ssize_t jSize = static_cast<ssize_t>(_accessor.size().y);
		ssize_t kSize = static_cast<ssize_t>(_accessor.size().z);
		R fx, fy, fz;

		assert(_gridSpacing.x > std::numeric_limits<R>::epsilon() &&
			_gridSpacing.y > std::numeric_limits<R>::epsilon() &&
			_gridSpacing.z > std::numeric_limits<R>::epsilon());
		Vector3<R> normalizedX = (x - _origin) / _gridSpacing;

		getBarycentric(normalizedX.x, 0, iSize - 1, &i, &fx);
		getBarycentric(normalizedX.y, 0, jSize - 1, &j, &fy);
		getBarycentric(normalizedX.z, 0, kSize - 1, &k, &fz);

		ssize_t is[4] = {
			std::max(i - 1, kZeroSSize),
			i,
			std::min(i + 1, iSize - 1),
			std::min(i + 2, iSize - 1)
		};
		ssize_t js[4] = {
			std::max(j - 1, kZeroSSize),
			j,
			std::min(j + 1, jSize - 1),
			std::min(j + 2, jSize - 1)
		};
		ssize_t ks[4] = {
			std::max(k - 1, kZeroSSize),
			k,
			std::min(k + 1, kSize - 1),
			std::min(k + 2, kSize - 1)
		};

		T kValues[4];

		for (int kk = 0; kk < 4; ++kk) 
		{
			T jValues[4];
			for (int jj = 0; jj < 4; ++jj)
			{
				jValues[jj] = monotonicCatmullRom(
					_accessor(is[0], js[jj], ks[kk]),
					_accessor(is[1], js[jj], ks[kk]),
					_accessor(is[2], js[jj], ks[kk]),
					_accessor(is[3], js[jj], ks[kk]),
					fx);
			}
			kValues[kk] = monotonicCatmullRom(jValues[0], jValues[1], jValues[2], jValues[3], fy);
		}

		return monotonicCatmullRom(kValues[0], kValues[1], kValues[2], kValues[3], fz);
	}

	template <typename T, typename R>
	std::function<T(const Vector3<R>&)> CubicArraySampler<T, R, 3>::functor() const 
	{
		CubicArraySampler sampler(*this);
		return std::bind(&CubicArraySampler::operator(), sampler, std::placeholders::_1);
	}

}
