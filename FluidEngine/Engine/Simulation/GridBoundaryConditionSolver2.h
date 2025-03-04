#pragma once

#include <memory>

#include "Collider2.h"
#include "../Utils/Constants.h"
#include "../Math/ScalarField2.h"
#include "../Geometry/FaceCenteredGrid2.h"

namespace Engine
{
	//!
	//! \brief Abstract base class for 2-D boundary condition solver for grids.
	//!
	//! This is a helper class to constrain the 2-D velocity field with given
	//! collider object. It also determines whether to open any domain boundaries.
	//! To control the friction level, tune the collider parameter.
	//!
	class GridBoundaryConditionSolver2
	{
	public:
		//! Default constructor.
		GridBoundaryConditionSolver2();

		//! Default destructor.
		virtual ~GridBoundaryConditionSolver2();

		//! Returns associated collider.
		const Collider2Ptr& collider() const;

		//!
		//! \brief Applies new collider and build the internals.
		//!
		//! This function is called to apply new collider and build the internal
		//! cache. To provide a hint to the cache, info for the expected velocity
		//! grid that will be constrained is provided.
		//!
		//! \param newCollider New collider to apply.
		//! \param gridSize Size of the velocity grid to be constrained.
		//! \param gridSpacing Grid spacing of the velocity grid to be constrained.
		//! \param gridOrigin Origin of the velocity grid to be constrained.
		//!
		void updateCollider(
			const Collider2Ptr& newCollider,
			const Size2& gridSize,
			const Vector2D& gridSpacing,
			const Vector2D& gridOrigin);

		//! Returns the closed domain boundary flag.
		int closedDomainBoundaryFlag() const;

		//! Sets the closed domain boundary flag.
		void setClosedDomainBoundaryFlag(int flag);

		//!
		//! Constrains the velocity field to conform the collider boundary.
		//!
		//! \param velocity Input and output velocity grid.
		//! \param extrapolationDepth Number of inner-collider grid cells that
		//!     velocity will get extrapolated.
		//!
		virtual void constrainVelocity(FaceCenteredGrid2* velocity, unsigned int extrapolationDepth = 5) = 0;

		//! Returns the signed distance field of the collider.
		virtual ScalarField2Ptr colliderSdf() const = 0;

		//! Returns the velocity field of the collider.
		virtual VectorField2Ptr colliderVelocityField() const = 0;

	protected:
		//! Invoked when a new collider is set.
		virtual void onColliderUpdated(
			const Size2& gridSize,
			const Vector2D& gridSpacing,
			const Vector2D& gridOrigin) = 0;

		//! Returns the size of the velocity grid to be constrained.
		const Size2& gridSize() const;

		//! Returns the spacing of the velocity grid to be constrained.
		const Vector2D& gridSpacing() const;

		//! Returns the origin of the velocity grid to be constrained.
		const Vector2D& gridOrigin() const;

	private:
		Size2 _gridSize;
		Vector2D _gridSpacing;
		Vector2D _gridOrigin;
		Collider2Ptr _collider;
		int _closedDomainBoundaryFlag = kDirectionAll;

	};

	//! Shared pointer type for the GridBoundaryConditionSolver2.
	typedef std::shared_ptr<GridBoundaryConditionSolver2> GridBoundaryConditionSolver2Ptr;

}
