
#include "Engine/Geometry/Box2.h"
#include "Engine/Simulation/ColliderSet2.h"
#include "Engine/Simulation/RigidBodyCollider2.h"
#include <gtest/gtest.h>
#include <vector>

using namespace Engine;

TEST(ColliderSet2, Constructors) {
	auto box1 = Box2::builder()
		.withLowerCorner({ 0, 1 })
		.withUpperCorner({ 1, 2 })
		.makeShared();

	auto box2 = Box2::builder()
		.withLowerCorner({ 2, 3 })
		.withUpperCorner({ 3, 4 })
		.makeShared();

	auto col1 = RigidBodyCollider2::builder()
		.withSurface(box1)
		.makeShared();

	auto col2 = RigidBodyCollider2::builder()
		.withSurface(box2)
		.makeShared();

	ColliderSet2 colSet1;
	EXPECT_EQ(0u, colSet1.numberOfColliders());

	ColliderSet2 colSet2({ col1, col2 });
	EXPECT_EQ(2u, colSet2.numberOfColliders());
	EXPECT_EQ(col1, colSet2.collider(0));
	EXPECT_EQ(col2, colSet2.collider(1));
}

TEST(ColliderSet2, Builder) {
	auto box1 = Box2::builder()
		.withLowerCorner({ 0, 1 })
		.withUpperCorner({ 1, 2 })
		.makeShared();

	auto box2 = Box2::builder()
		.withLowerCorner({ 2, 3 })
		.withUpperCorner({ 3, 4 })
		.makeShared();

	auto col1 = RigidBodyCollider2::builder()
		.withSurface(box1)
		.makeShared();

	auto col2 = RigidBodyCollider2::builder()
		.withSurface(box2)
		.makeShared();

	auto colSet1 = ColliderSet2::builder().makeShared();
	EXPECT_EQ(0u, colSet1->numberOfColliders());

	auto colSet2 = ColliderSet2::builder()
		.withColliders({ col1, col2 })
		.makeShared();
	EXPECT_EQ(2u, colSet2->numberOfColliders());
	EXPECT_EQ(col1, colSet2->collider(0));
	EXPECT_EQ(col2, colSet2->collider(1));

	auto colSet3 = ColliderSet2::builder().build();
	EXPECT_EQ(0u, colSet3.numberOfColliders());
}


