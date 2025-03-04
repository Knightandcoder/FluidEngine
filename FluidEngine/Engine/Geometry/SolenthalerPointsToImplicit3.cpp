#include "SolenthalerPointsToImplicit3.h"

#include "../Math/SingularValueDecomposition.h"
#include "../Simulation/ParticleSystemData3.h"

namespace Engine
{
	inline double k(double s) { return std::max(0.0, cubic(1.0 - s * s)); }

	inline double gradientK(double s) 
	{ 
		if (fabs(s) < 1.0)
			return -6.0 * s * square(1 - s * s);
		else
			return 0.0;
	}

	inline double dkdx(double s, double kernelRadius, double dx)
	{
		return gradientK(s) * 1.0 / kernelRadius * dx / (s * kernelRadius);
	}

	inline double dkdy(double s, double kernelRadius, double dy)
	{
		return gradientK(s) * 1.0 / kernelRadius * dy / (s * kernelRadius);
	}

	inline double dkdz(double s, double kernelRadius, double dz)
	{
		return gradientK(s) * 1.0 / kernelRadius * dz / (s * kernelRadius);
	}

	inline double f(double largestEigenValue, double tLow, double tHigh)
	{
		if (largestEigenValue < tLow)
			return 1.0;
		double lambda = (tHigh - largestEigenValue) / (tHigh - tLow);
		return cubic(lambda) - 3 * square(lambda) + 3 * lambda;
	}

	SolenthalerPointsToImplicit3::SolenthalerPointsToImplicit3(double kernelRadius,
		double cutOffThreshold, bool isOutputSdf)
		: _kernelRadius(kernelRadius), _cutOffThreshold(cutOffThreshold), _isOutputSdf(isOutputSdf) {}

	void SolenthalerPointsToImplicit3::convert(const ConstArrayAccessor1<Vector3D>& points,
		ScalarGrid3 * output) const
	{
		if (output == nullptr)
		{
			std::cout << "Null scalar grid output pointer provided.\n";
			return;
		}

		const auto res = output->resolution();
		if (res.x * res.y * res.z == 0)
		{
			std::cout << "Empty grid is provided.\n";
			return;
		}

		const auto bbox = output->boundingBox();
		if (bbox.isEmpty())
		{
			std::cout << "Empty domain is provided.\n";
			return;
		}

		ParticleSystemData3 particles;
		particles.addParticles(points);
		particles.buildNeighborSearcher(_kernelRadius);
		//�� it's improved based on Zhu and Bridson's method.
		const auto neighborSearcher = particles.neighborSearcher();
		const double isoContValue = _cutOffThreshold * _kernelRadius;
		auto temp = output->clone();
		temp->fill([&](const Vector3D& x) -> double
		{
			Vector3D xAvg;
			double wSum = 0.0;

			double wGradX = 0.0;
			double wGradY = 0.0;
			double wGradZ = 0.0;

			double wXSum = 0.0;
			double wYSum = 0.0;
			double wZSum = 0.0;

			double w_plus_xGradX_sum = 0;
			double xGradY_sum = 0;
			double xGradZ_sum = 0;

			double yGradX_sum = 0;
			double w_plus_yGradY_sum = 0;
			double yGradZ_sum = 0;

			double zGradX_sum = 0;
			double zGradY_sum = 0;
			double w_plus_zGradZ_sum = 0;

			const auto func = [&](size_t, const Vector3D& xi)
			{
				double s = (x - xi).length() / _kernelRadius;
				const double wi = k(s);
				const double ks = wi;
				wSum += wi;
				xAvg += wi * xi;

				const double gradk_dx = dkdx(s, _kernelRadius, x.x - xi.x);
				const double gradk_dy = dkdy(s, _kernelRadius, x.y - xi.y);
				const double gradk_dz = dkdz(s, _kernelRadius, x.z - xi.z);

				wGradX += gradk_dx;
				wGradY += gradk_dy;
				wGradZ += gradk_dz;

				wXSum += x.x * ks;
				wYSum += x.y * ks;
				wZSum += x.z * ks;

				w_plus_xGradX_sum += (ks + x.x * gradk_dx);
				xGradY_sum += x.x * gradk_dy;
				xGradZ_sum += x.x * gradk_dz;

				yGradX_sum += x.y * gradk_dx;
				w_plus_yGradY_sum += ks + x.y * gradk_dy;
				yGradZ_sum += x.y * gradk_dz;

				zGradX_sum += x.z * gradk_dx;
				zGradY_sum += x.z * gradk_dy;
				w_plus_zGradZ_sum += ks + x.z * gradk_dz;

			};
			neighborSearcher->forEachNearbyPoint(x, _kernelRadius, func);

			// Jacobian matrix.
			double dy1_dx, dy1_dy, dy1_dz;
			double dy2_dx, dy2_dy, dy2_dz;
			double dy3_dx, dy3_dy, dy3_dz;
			dy1_dx = (-1) * 1.0 / square(wSum) * wGradX * wXSum + 1.0 / wSum * w_plus_xGradX_sum;
			dy1_dy = (-1) * 1.0 / square(wSum) * wGradY * wXSum + 1.0 / wSum * xGradY_sum;
			dy1_dz = (-1) * 1.0 / square(wSum) * wGradZ * wXSum + 1.0 / wSum * xGradZ_sum;

			dy2_dx = (-1) * 1.0 / square(wSum) * wGradX * wYSum + 1.0 / wSum * yGradX_sum;
			dy2_dy = (-1) * 1.0 / square(wSum) * wGradY * wYSum + 1.0 / wSum * w_plus_yGradY_sum;
			dy2_dz = (-1) * 1.0 / square(wSum) * wGradZ * wYSum + 1.0 / wSum * yGradZ_sum;

			dy3_dx = (-1) * 1.0 / square(wSum) * wGradX * wZSum + 1.0 / wSum * zGradX_sum;
			dy3_dy = (-1) * 1.0 / square(wSum) * wGradY * wZSum + 1.0 / wSum * zGradY_sum;
			dy3_dz = (-1) * 1.0 / square(wSum) * wGradZ * wZSum + 1.0 / wSum * w_plus_zGradZ_sum;

			Matrix3x3D cov;
			cov.set(dy1_dx, dy1_dy, dy1_dz, dy2_dx, dy2_dy, dy2_dz,dy3_dx, dy3_dy, dy3_dz);

			// SVD
			Matrix3x3D u;
			Vector3D v;
			Matrix3x3D w;
			svd(cov, u, v, w);

			//! largest eigenvalue of Jacobi matrix.
			double largestEigenValue = v.max();

			double fValue = f(largestEigenValue, 0.4, 3.5);

			if (wSum > 0.0)
			{
				xAvg /= wSum;
				return (x - xAvg).length() - isoContValue * fValue;
			}
			else
				return output->boundingBox().diagonalLength();
		});

		//if (_isOutputSdf) {
		//	FmmLevelSetSolver3 solver;
		//	solver.reinitialize(*temp, kMaxD, output);
		//}
		//else 
		temp->swap(output);
	}

}