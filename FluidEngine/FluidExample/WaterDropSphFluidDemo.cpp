#include "WaterDropSphFluidDemo.h"

#include "Engine/Geometry/Plane3.h"
#include "Engine/Geometry/Sphere3.h"
#include "Engine/Geometry/ImplicitSurfaceSet3.h"
#include "Engine/Geometry/BoundingBox3.h"
#include "Engine/Geometry/Box3.h"
#include "Engine/Simulation/SphSolver3.h"
#include "Engine/Simulation/VolumeParticleEmitter3.h"
#include "Engine/Simulation/RigidBodyCollider3.h"

#include <sstream>

using namespace Engine;

void WaterDropSphFluidDemo::setup(double radius)
{
	minX = minY = minZ = 0.0;
	maxX = 1;
	maxY = 2;
	maxZ = 1;

	std::stringstream ss;
	ss << "<texture type=\"checkerboard\" id=\"__planetex\">";
	ss << "<rgb name=\"color0\" value=\"0.4\"/>";
	ss << "<rgb name=\"color1\" value=\"0.2\"/>";
	ss << "<float name=\"uscale\" value=\"8.0\"/>";
	ss << "<float name=\"vscale\" value=\"8.0\"/>";
	ss << "<float name=\"uoffset\" value=\"0.0\"/>";
	ss << "<float name=\"voffset\" value=\"0.0\"/>";
	ss << "</texture>";
	ss << "<shape type=\"obj\">";
	ss << "<string name=\"filename\" value=\"plane.obj\"/>";
	ss << "<bsdf type=\"diffuse\">";
	ss << "<ref name=\"reflectance\" id=\"__planetex\"/>";
	ss << "</bsdf>";
	ss << "<transform name=\"toWorld\">";
	ss << "<scale value=\"10.0\"/>";
	ss << "</transform>";
	ss << "</shape>";
	_sceneXml = ss.str();

	BoundingBox3D domain(Vector3D(), Vector3D(1, 2, 1));

	_origin = Vector3D(minX, minY, minZ);
	_resolution = Size3(domain.width() / radius + 5, domain.height() / radius + 5, domain.depth() / radius + 5);

	auto solver = SphSolver3::builder()
		.withTargetDensity(1000.0)
		.withTargetSpacing(radius)
		.makeShared();

	solver->setPseudoViscosityCoefficient(0.0);

	// Build emitter.
	BoundingBox3D sourceBound(domain);
	sourceBound.expand(-radius);

	auto plane = Plane3::builder()
		.withNormal({ 0,1,0 })
		.withPoint({ 0,0.10*domain.height(), 0 })
		.makeShared();

	auto sphere = Sphere3::builder()
		.withCenter(domain.midPoint())
		.withRadius(0.15 * domain.width())
		.makeShared();

	auto surfaceSet = ImplicitSurfaceSet3::builder()
		.withExplicitSurfaces({ plane, sphere })
		.makeShared();

	auto emitter = VolumeParticleEmitter3::builder()
		.withImplicitSurface(surfaceSet)
		.withSpacing(radius)
		.withMaxRegion(sourceBound)
		.withIsOneShot(true)
		.makeShared();

	solver->setEmitter(emitter);

	// Build collider.
	auto box = Box3::builder()
		.withIsNormalFlipped(true)
		.withBoundingBox(domain)
		.makeShared();
	auto collider = RigidBodyCollider3::builder()
		.withSurface(box).makeShared();

	solver->setCollider(collider);

	_solver = solver;
}
