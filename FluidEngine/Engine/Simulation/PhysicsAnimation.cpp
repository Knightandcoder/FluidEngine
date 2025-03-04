#include "PhysicsAnimation.h"

#include "Timer.h"
#include "../Utils/Constants.h"

#include <limits>
#include <iostream>

namespace Engine
{
	PhysicsAnimation::PhysicsAnimation() { _currentFrame.index = -1; }

	PhysicsAnimation::~PhysicsAnimation() {}

	bool PhysicsAnimation::isUsingFixedSubTimeSteps() const { return _isUsingFixedSubTimeSteps; }

	void PhysicsAnimation::setIsUsingFixedSubTimeSteps(bool isUsing) { _isUsingFixedSubTimeSteps = isUsing; }

	unsigned int PhysicsAnimation::numberOfFixedSubTimeSteps() const { return _numberOfFixedSubTimeSteps; }

	void PhysicsAnimation::setNumberOfFixedSubTimeSteps(unsigned int numberOfSteps) 
	{
		_numberOfFixedSubTimeSteps = numberOfSteps;
	}

	void PhysicsAnimation::advanceSingleFrame() 
	{
		Frame f = _currentFrame;
		update(++f);
	}

	Frame PhysicsAnimation::currentFrame() const { return _currentFrame; }

	void PhysicsAnimation::setCurrentFrame(const Frame& frame) { _currentFrame = frame; }

	double PhysicsAnimation::currentTimeInSeconds() const { return _currentTime; }

	unsigned int PhysicsAnimation::numberOfSubTimeSteps(double timeIntervalInSeconds) const 
	{
		// Returns number of fixed sub-timesteps by default
		return _numberOfFixedSubTimeSteps;
	}

	void PhysicsAnimation::onUpdate(const Frame& frame) {
		if (frame.index > _currentFrame.index) 
		{
			initialize();
			int32_t numberOfFrames = frame.index - _currentFrame.index;
			for (int32_t i = 0; i < numberOfFrames; ++i) 
				advanceTimeStep(frame.timeIntervalInSeconds);
			_currentFrame = frame;
		}
	}

	void PhysicsAnimation::advanceTimeStep(double timeIntervalInSeconds) 
	{
		_currentTime = _currentFrame.timeInSeconds();

		if (_isUsingFixedSubTimeSteps) 
		{
			std::cout << "Using fixed sub-timesteps: " << _numberOfFixedSubTimeSteps << std::endl;

			// Perform fixed time-stepping
			const double actualTimeInterval = timeIntervalInSeconds /
				static_cast<double>(_numberOfFixedSubTimeSteps);

			for (unsigned int i = 0; i < _numberOfFixedSubTimeSteps; ++i) 
			{
				std::cout << "Begin onAdvanceTimeStep: " << actualTimeInterval
					<< " (1/" << 1.0 / actualTimeInterval << ") seconds" << std::endl;

				Timer timer;
				onAdvanceTimeStep(actualTimeInterval);

				std::cout << "End onAdvanceTimeStep (took "
					<< timer.durationInSeconds() << " seconds)" << std::endl;

				_currentTime += actualTimeInterval;
			}
		}
		else 
		{
			std::cout << "Using adaptive sub-timesteps\n";

			// Perform adaptive time-stepping
			double remainingTime = timeIntervalInSeconds;
			while (remainingTime > kEpsilonD) 
			{
				unsigned int numSteps = numberOfSubTimeSteps(remainingTime);
				double actualTimeInterval = remainingTime / static_cast<double>(numSteps);

				std::cout << "Number of remaining sub-timesteps: " << numSteps << std::endl;

				std::cout << "Begin onAdvanceTimeStep: " << actualTimeInterval
					<< " (1/" << 1.0 / actualTimeInterval << ") seconds\n";

				Timer timer;
				onAdvanceTimeStep(actualTimeInterval);

				std::cout << "End onAdvanceTimeStep (took "
					<< timer.durationInSeconds() << " seconds)\n";

				remainingTime -= actualTimeInterval;
				_currentTime += actualTimeInterval;
			}
		}
	}

	void PhysicsAnimation::initialize() 
	{ 
		if (!_isInitialized)
		{
			onInitialize();
			_isInitialized = true;
		}
	}

	void PhysicsAnimation::onInitialize() { }
}