#include <gtest/gtest.h>
#include "Engine/Simulation/GridFractionalBoundaryConditionSolver3.h"
#include "Engine/Geometry/Plane3.h"
#include "Engine/Simulation/RigidBodyCollider3.h"

using namespace Engine;

TEST(GridFractionalBoundaryConditionSolver3, ClosedDomain) {
	GridFractionalBoundaryConditionSolver3 bndSolver;
	Size3 gridSize(10, 10, 10);
	Vector3D gridSpacing(1.0, 1.0, 1.0);
	Vector3D gridOrigin(-5.0, -5.0, -5.0);

	bndSolver.updateCollider(nullptr, gridSize, gridSpacing, gridOrigin);

	FaceCenteredGrid3 velocity(gridSize, gridSpacing, gridOrigin);
	velocity.fill(Vector3D(1.0, 1.0, 1.0));

	bndSolver.constrainVelocity(&velocity);

	velocity.forEachUIndex([&](size_t i, size_t j, size_t k) {
		if (i == 0 || i == gridSize.x) {
			EXPECT_DOUBLE_EQ(0.0, velocity.u(i, j, k));
		}
		else {
			EXPECT_DOUBLE_EQ(1.0, velocity.u(i, j, k));
		}
	});

	velocity.forEachVIndex([&](size_t i, size_t j, size_t k) {
		if (j == 0 || j == gridSize.y) {
			EXPECT_DOUBLE_EQ(0.0, velocity.v(i, j, k));
		}
		else {
			EXPECT_DOUBLE_EQ(1.0, velocity.v(i, j, k));
		}
	});

	velocity.forEachWIndex([&](size_t i, size_t j, size_t k) {
		if (k == 0 || k == gridSize.z) {
			EXPECT_DOUBLE_EQ(0.0, velocity.w(i, j, k));
		}
		else {
			EXPECT_DOUBLE_EQ(1.0, velocity.w(i, j, k));
		}
	});
}

TEST(GridFractionalBoundaryConditionSolver3, OpenDomain) {
	GridFractionalBoundaryConditionSolver3 bndSolver;
	Size3 gridSize(10, 10, 10);
	Vector3D gridSpacing(1.0, 1.0, 1.0);
	Vector3D gridOrigin(-5.0, -5.0, -5.0);

	// Partially open domain
	bndSolver.setClosedDomainBoundaryFlag(
		kDirectionLeft | kDirectionUp | kDirectionFront);
	bndSolver.updateCollider(nullptr, gridSize, gridSpacing, gridOrigin);

	FaceCenteredGrid3 velocity(gridSize, gridSpacing, gridOrigin);
	velocity.fill(Vector3D(1.0, 1.0, 1.0));

	bndSolver.constrainVelocity(&velocity);

	velocity.forEachUIndex([&](size_t i, size_t j, size_t k) {
		if (i == 0) {
			EXPECT_DOUBLE_EQ(0.0, velocity.u(i, j, k));
		}
		else {
			EXPECT_DOUBLE_EQ(1.0, velocity.u(i, j, k));
		}
	});

	velocity.forEachVIndex([&](size_t i, size_t j, size_t k) {
		if (j == gridSize.y) {
			EXPECT_DOUBLE_EQ(0.0, velocity.v(i, j, k));
		}
		else {
			EXPECT_DOUBLE_EQ(1.0, velocity.v(i, j, k));
		}
	});

	velocity.forEachWIndex([&](size_t i, size_t j, size_t k) {
		if (k == gridSize.z) {
			EXPECT_DOUBLE_EQ(0.0, velocity.w(i, j, k));
		}
		else {
			EXPECT_DOUBLE_EQ(1.0, velocity.w(i, j, k));
		}
	});
}

