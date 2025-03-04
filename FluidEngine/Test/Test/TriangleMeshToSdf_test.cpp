#include "Engine/Geometry/TriangleMeshToSdf.h"

#include <gtest/gtest.h>
#include "Engine/Geometry/Box3.h"
#include "Engine/Geometry/CellCenteredScalarGrid3.h"

using namespace Engine;

TEST(TriangleMeshToSdf, TriangleMeshToSdf) {
	TriangleMesh3 mesh;

	// Build a cube
	mesh.addPoint({ 0.0, 0.0, 0.0 });
	mesh.addPoint({ 0.0, 0.0, 1.0 });
	mesh.addPoint({ 0.0, 1.0, 0.0 });
	mesh.addPoint({ 0.0, 1.0, 1.0 });
	mesh.addPoint({ 1.0, 0.0, 0.0 });
	mesh.addPoint({ 1.0, 0.0, 1.0 });
	mesh.addPoint({ 1.0, 1.0, 0.0 });
	mesh.addPoint({ 1.0, 1.0, 1.0 });

	mesh.addPointTriangle({ 0, 1, 3 });
	mesh.addPointTriangle({ 0, 3, 2 });
	mesh.addPointTriangle({ 4, 6, 7 });
	mesh.addPointTriangle({ 4, 7, 5 });
	mesh.addPointTriangle({ 0, 4, 5 });
	mesh.addPointTriangle({ 0, 5, 1 });
	mesh.addPointTriangle({ 2, 3, 7 });
	mesh.addPointTriangle({ 2, 7, 6 });
	mesh.addPointTriangle({ 0, 2, 6 });
	mesh.addPointTriangle({ 0, 6, 4 });
	mesh.addPointTriangle({ 1, 5, 7 });
	mesh.addPointTriangle({ 1, 7, 3 });

	CellCenteredScalarGrid3 grid(
		3, 3, 3,
		1.0, 1.0, 1.0,
		-1.0, -1.0, -1.0);

	triangleMeshToSdf(mesh, &grid, 10);

	Box3 box(Vector3D(), Vector3D(1.0, 1.0, 1.0));

	auto gridPos = grid.dataPosition();
	grid.forEachDataPointIndex(
		[&](size_t i, size_t j, size_t k) {
		auto pos = gridPos(i, j, k);
		double ans = box.closestDistance(pos);
		ans *= box.bound.contains(pos) ? -1.0 : 1.0;
		EXPECT_DOUBLE_EQ(ans, grid(i, j, k));
	});
}

