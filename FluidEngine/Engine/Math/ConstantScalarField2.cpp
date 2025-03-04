#include "ConstantScalarField2.h"

namespace Engine
{
	ConstantScalarField2::ConstantScalarField2(double value) : _value(value) { }

	double ConstantScalarField2::sample(const Vector2D& x) const { return _value; }

	std::function<double(const Vector2D&)> ConstantScalarField2::sampler() const 
	{
		double value = _value;
		return [value](const Vector2D&) -> double {
			return value;
		};
	}

	ConstantScalarField2::Builder ConstantScalarField2::builder() { return Builder(); }


	ConstantScalarField2::Builder&
		ConstantScalarField2::Builder::withValue(double value) 
	{
		_value = value;
		return *this;
	}

	ConstantScalarField2 ConstantScalarField2::Builder::build() const 
	{
		return ConstantScalarField2(_value);
	}

	ConstantScalarField2Ptr ConstantScalarField2::Builder::makeShared() const
	{
		return std::shared_ptr<ConstantScalarField2>(
			new ConstantScalarField2(_value),
			[](ConstantScalarField2* obj) {
			delete obj;
		});
	}

}