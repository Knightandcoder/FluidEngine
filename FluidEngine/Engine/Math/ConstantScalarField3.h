#pragma once

#include "ScalarField3.h"

namespace Engine
{
	//! 3-D constant scalar field.
	class ConstantScalarField3 final : public ScalarField3 
	{
	public:
		class Builder;

		//! Constructs a constant scalar field with given \p value.
		explicit ConstantScalarField3(double value);

		//! Returns the sampled value at given position \p x.
		double sample(const Vector3D& x) const override;

		//! Returns the sampler function.
		std::function<double(const Vector3D&)> sampler() const override;

		//! Returns builder fox ConstantScalarField3.
		static Builder builder();

	private:
		double _value = 0.0;
	};

	//! Shared pointer for the ConstantScalarField3 type.
	typedef std::shared_ptr<ConstantScalarField3> ConstantScalarField3Ptr;


	//!
	//! \brief Front-end to create ConstantScalarField3 objects step by step.
	//!
	class ConstantScalarField3::Builder final 
	{
	public:
		//! Returns builder with value.
		Builder& withValue(double value);

		//! Builds ConstantScalarField3.
		ConstantScalarField3 build() const;

		//! Builds shared pointer of ConstantScalarField3 instance.
		ConstantScalarField3Ptr makeShared() const;

	private:
		double _value = 0.0;
	};
}