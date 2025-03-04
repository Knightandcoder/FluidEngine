#include <gtest/gtest.h>

#include "Engine/Geometry/ImplicitSurfaceSet2.h"
#include "Engine/Geometry/ImplicitSurfaceSet3.h"
#include "Engine/Simulation/LevelSetLiquidSolver2.h"
#include "Engine/Simulation/LevelSetLiquidSolver3.h"
#include "Engine/Geometry/Sphere2.h"
#include "Engine/Geometry/Sphere3.h"
#include "Engine/Geometry/SurfaceToImplicit2.h"
#include "Engine/Geometry/SurfaceToImplicit3.h"

using namespace Engine;

TEST(LevelSetLiquidSolver2, ComputeVolume) {
	LevelSetLiquidSolver2 solver;
	solver.setIsGlobalCompensationEnabled(true);

	auto data = solver.gridSystemData();
	double dx = 1.0 / 32.0;
	data->resize(Size2(32, 64), Vector2D(dx, dx), Vector2D());

	// Source setting
	const double radius = 0.15;
	BoundingBox2D domain = data->boundingBox();
	ImplicitSurfaceSet2 surfaceSet;
	surfaceSet.addExplicitSurface(
		std::make_shared<Sphere2>(domain.midPoint(), radius));

	auto sdf = solver.signedDistanceField();
	sdf->fill([&](const Vector2D& x) {
		return surfaceSet.signedDistance(x);
	});

	// Measure the volume
	double volume = solver.computeVolume();
	const double ans = square(radius) * kPiD;

	EXPECT_NEAR(ans, volume, 0.001);
}

TEST(LevelSetLiquidSolver3, ComputeVolume) {
	LevelSetLiquidSolver3 solver;
	solver.setIsGlobalCompensationEnabled(true);

	auto data = solver.gridSystemData();
	double dx = 1.0 / 32.0;
	data->resize(Size3(32, 64, 32), Vector3D(dx, dx, dx), Vector3D());

	// Source setting
	const double radius = 0.15;
	BoundingBox3D domain = data->boundingBox();
	ImplicitSurfaceSet3 surfaceSet;
	surfaceSet.addExplicitSurface(
		std::make_shared<Sphere3>(domain.midPoint(), radius));

	auto sdf = solver.signedDistanceField();
	sdf->fill([&](const Vector3D& x) {
		return surfaceSet.signedDistance(x);
	});

	// Measure the volume
	double volume = solver.computeVolume();
	const double ans = 4.0 / 3.0 * cubic(radius) * kPiD;

	EXPECT_NEAR(ans, volume, 0.001);
}


