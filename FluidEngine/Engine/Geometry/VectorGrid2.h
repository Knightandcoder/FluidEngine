#pragma once

#include <memory>
#include <vector>

#include "Grid2.h"
#include "../Utils/Parallel.h"
#include "../Math/VectorField2.h"
#include "../Math/ArrayAccessor2.h"

namespace Engine
{
	//! Abstract base class for 2-D vector grid structure.
	class VectorGrid2 : public VectorField2, public Grid2
	{
	public:

		//! Read-write array accessor type.
		typedef ArrayAccessor2<Vector2D> VectorDataAccessor;

		//! Read-only array accessor type.
		typedef ConstArrayAccessor2<Vector2D> ConstVectorDataAccessor;

		//! Constructs an empty grid.
		VectorGrid2();

		//! Default destructor.
		virtual ~VectorGrid2();

		//! Clears the contents of the grid.
		void clear();

		//! Resizes the grid using given parameters.
		void resize(size_t resolutionX, size_t resolutionY,
			double gridSpacingX = 1.0, double gridSpacingY = 1.0,
			double originX = 0.0, double originY = 0.0,
			double initialValueX = 0.0, double initialValueY = 0.0);

		//! Resizes the grid using given parameters.
		void resize(const Size2& resolution,
			const Vector2D& gridSpacing = Vector2D(1, 1),
			const Vector2D& origin = Vector2D(),
			const Vector2D& initialValue = Vector2D());

		//! Resizes the grid using given parameters.
		void resize(double gridSpacingX, double gridSpacingY, double originX, double originY);

		//! Resizes the grid using given parameters.
		void resize(const Vector2D& gridSpacing, const Vector2D& origin);

		//! Fills the grid with given value.
		virtual void fill(const Vector2D& value, ExecutionPolicy policy = ExecutionPolicy::kParallel) = 0;

		//! Fills the grid with given position-to-value mapping function.
		virtual void fill(const std::function<Vector2D(const Vector2D&)>& func,
			ExecutionPolicy policy = ExecutionPolicy::kParallel) = 0;

		//! Returns the copy of the grid instance.
		virtual std::shared_ptr<VectorGrid2> clone() const = 0;

	protected:
		//!
		//! \brief Invoked when the resizing happens.
		//!
		//! This callback function is called when the grid gets resized. The
		//! overriding class should allocate the internal storage based on its
		//! data layout scheme.
		//!
		virtual void onResize(const Size2& resolution, const Vector2D& gridSpacing,
			const Vector2D& origin, const Vector2D& initialValue) = 0;

	};

	//! Shared pointer for the VectorGrid2 type.
	typedef std::shared_ptr<VectorGrid2> VectorGrid2Ptr;

	//! Abstract base class for 2-D vector grid builder.
	class VectorGridBuilder2 
	{
	public:
		//! Creates a builder.
		VectorGridBuilder2();

		//! Default destructor.
		virtual ~VectorGridBuilder2();

		//! Returns 2-D vector grid with given parameters.
		virtual VectorGrid2Ptr build(const Size2& resolution,
			const Vector2D& gridSpacing,
			const Vector2D& gridOrigin,
			const Vector2D& initialVal) const = 0;
	};

	//! Shared pointer for the VectorGridBuilder2 type.
	typedef std::shared_ptr<VectorGridBuilder2> VectorGridBuilder2Ptr;

}
