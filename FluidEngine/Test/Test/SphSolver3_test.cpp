#include "Engine/Simulation/SphSolver3.h"
#include <gtest/gtest.h>

using namespace Engine;

TEST(SphSolver3, UpdateEmpty) {
	// Empty solver test
	SphSolver3 solver;
	Frame frame(0, 0.01);
	solver.update(frame++);
	solver.update(frame);
}

TEST(SphSolver3, Parameters) {
	SphSolver3 solver;

	solver.setEosExponent(5.0);
	EXPECT_DOUBLE_EQ(5.0, solver.eosExponent());

	solver.setEosExponent(-1.0);
	EXPECT_DOUBLE_EQ(1.0, solver.eosExponent());

	solver.setNegativePressureScale(0.3);
	EXPECT_DOUBLE_EQ(0.3, solver.negativePressureScale());

	solver.setNegativePressureScale(-1.0);
	EXPECT_DOUBLE_EQ(0.0, solver.negativePressureScale());

	solver.setNegativePressureScale(3.0);
	EXPECT_DOUBLE_EQ(1.0, solver.negativePressureScale());

	solver.setViscosityCoefficient(0.3);
	EXPECT_DOUBLE_EQ(0.3, solver.viscosityCoefficient());

	solver.setViscosityCoefficient(-1.0);
	EXPECT_DOUBLE_EQ(0.0, solver.viscosityCoefficient());

	solver.setPseudoViscosityCoefficient(0.3);
	EXPECT_DOUBLE_EQ(0.3, solver.pseudoViscosityCoefficient());

	solver.setPseudoViscosityCoefficient(-1.0);
	EXPECT_DOUBLE_EQ(0.0, solver.pseudoViscosityCoefficient());

	solver.setSpeedOfSound(0.3);
	EXPECT_DOUBLE_EQ(0.3, solver.speedOfSound());

	solver.setSpeedOfSound(-1.0);
	EXPECT_GT(solver.speedOfSound(), 0.0);

	solver.setTimeStepLimitScale(0.3);
	EXPECT_DOUBLE_EQ(0.3, solver.timeStepLimitScale());

	solver.setTimeStepLimitScale(-1.0);
	EXPECT_DOUBLE_EQ(0.0, solver.timeStepLimitScale());

	EXPECT_TRUE(solver.sphSystemData() != nullptr);
}


