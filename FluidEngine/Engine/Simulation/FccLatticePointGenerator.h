#pragma once

#include "PointGenerator3.h"

namespace Engine
{
	//!
	//! \brief Face-centered lattice points generator.
	//!
	//! \see http://en.wikipedia.org/wiki/Cubic_crystal_system
	//!      http://mathworld.wolfram.com/CubicClosePacking.html
	//!
	class FccLatticePointGenerator final : public PointGenerator3
	{
	public:
		//!
		//! \brief Invokes \p callback function for each FCC-lattice points inside
		//! \p boundingBox.
		//!
		//! This function iterates every FCC-lattice points inside \p boundingBox
		//! where \p spacing is the size of the unit cell of FCC structure.
		//!
		void forEachPoint(
			const BoundingBox3D& boundingBox,
			double spacing,
			const std::function<bool(const Vector3D&)>& callback) const override;

	};

	//! Shared pointer type for the FccLatticePointGenerator.
	typedef std::shared_ptr<FccLatticePointGenerator> FccLatticePointGeneratorPtr;

}
