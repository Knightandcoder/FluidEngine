#pragma once

#include <memory>

#include "VectorField3.h"

namespace Engine
{
	//! 3-D constant vector field.
	class ConstantVectorField3 final : public VectorField3 
	{
	public:
		class Builder;

		//! Constructs a constant vector field with given \p value.
		explicit ConstantVectorField3(const Vector3D& value);

		//! Returns the sampled value at given position \p x.
		Vector3D sample(const Vector3D& x) const override;

		//! Returns the sampler function.
		std::function<Vector3D(const Vector3D&)> sampler() const override;

		//! Returns builder fox ConstantVectorField3.
		static Builder builder();

	private:
		Vector3D _value;
	};

	//! Shared pointer for the ConstantVectorField3 type.
	typedef std::shared_ptr<ConstantVectorField3> ConstantVectorField3Ptr;


	//!
	//! \brief Front-end to create ConstantVectorField3 objects step by step.
	//!
	class ConstantVectorField3::Builder final 
	{
	public:
		//! Returns builder with value.
		Builder& withValue(const Vector3D& value);

		//! Builds ConstantVectorField3.
		ConstantVectorField3 build() const;

		//! Builds shared pointer of ConstantVectorField3 instance.
		ConstantVectorField3Ptr makeShared() const;

	private:
		Vector3D _value{ 0, 0, 0 };
	};

}