#pragma once

#include <memory>

#include "../Math/CustomVectorField3.h"
#include "GridBoundaryConditionSolver3.h"
#include "../Geometry/CellCenteredScalarGrid3.h"

namespace Engine
{
	//!
	//! \brief Fractional 3-D boundary condition solver for grids.
	//!
	//! This class constrains the velocity field by projecting the flow to the
	//! signed-distance field representation of the collider. This implementation
	//! should pair up with GridFractionalSinglePhasePressureSolver3 to provide
	//! sub-grid resolutional velocity projection.
	//!
	class GridFractionalBoundaryConditionSolver3 : public GridBoundaryConditionSolver3
	{
	public:
		//! Default constructor.
		GridFractionalBoundaryConditionSolver3();

		//! Default destructor.
		virtual ~GridFractionalBoundaryConditionSolver3();

		//!
		//! Constrains the velocity field to conform the collider boundary.
		//!
		//! \param velocity Input and output velocity grid.
		//! \param extrapolationDepth Number of inner-collider grid cells that
		//!     velocity will get extrapolated.
		//!
		void constrainVelocity(FaceCenteredGrid3* velocity, unsigned int extrapolationDepth = 5) override;

		//! Returns the signed distance field of the collider.
		ScalarField3Ptr colliderSdf() const override;

		//! Returns the velocity field of the collider.
		VectorField3Ptr colliderVelocityField() const override;

	protected:
		//! Invoked when a new collider is set.
		void onColliderUpdated(
			const Size3& gridSize,
			const Vector3D& gridSpacing,
			const Vector3D& gridOrigin) override;

	private:
		CellCenteredScalarGrid3Ptr _colliderSdf;
		CustomVectorField3Ptr _colliderVel;

	};

	//! Shared pointer type for the GridFractionalBoundaryConditionSolver3.
	typedef std::shared_ptr<GridFractionalBoundaryConditionSolver3> GridFractionalBoundaryConditionSolver3Ptr;

}
