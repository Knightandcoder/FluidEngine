#include "ParticleFluidDemo.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <algorithm>

#include "Engine/Utils/ArrayUtils.h"
#include "Engine/Geometry/MarchingCubes.h"
#include "Engine/Simulation/SphSystemData3.h"
#include "Engine/Geometry/SphPointsToImplicit3.h"
#include "Engine/Geometry/AnisotropicPointsToImplicit3.h"
#include "Engine/Geometry/SolenthalerPointsToImplicit3.h"
#include "Engine/Geometry/ZhuBridsonPointsToImplicit3.h"
#include "Engine/Geometry/VertexCenteredScalarGrid3.h"

using namespace Engine;

void ParticleFluidDemo::run(const std::string & rootDir, int numberOfFrames,
	const std::string & format, double fps)
{
	if (_solver == nullptr)
	{
		std::cout << "You have not set a solver to run.\n";
		return;
	}

	auto particles = _solver->particleSystemData();
	std::cout << "Number of particles: " << particles->numberOfParticles() << std::endl;

	Frame curFrame(_bFrame - 1, 1.0 / fps);
	_solver->setCurrentFrame(curFrame);
	for (Frame frame(_bFrame, 1.0 / fps); frame.index < (_bFrame + numberOfFrames); ++frame)
	{
		_solver->update(frame);
		if (format == "xyz")
			saveParticleAsXyz(particles, rootDir, frame.index);
		else if (format == "pos")
			saveParticleAsPos(particles, rootDir, frame.index);
		else if (format == "xml")
			saveParticleAsXml(particles, rootDir, frame.index);
		else if (format == "obj")
			saveParticleAsObj(particles, rootDir, frame.index);
	}
}

void ParticleFluidDemo::saveParticleAsObj(const Engine::ParticleSystemData3Ptr & particles, 
	const std::string & rootDir, int frameCnt)
{
	// convert to implicit surface.
	PointsToImplicit3Ptr converter;
	SphSystemData3 *data = reinterpret_cast<SphSystemData3*>(particles.get());

	double sZhuBridsonCutOffThreshold = 0.25;

	//converter = std::make_shared<SphPointsToImplicit3>(data->kernelRadius(), _sphCutOffDensity, false);
	//converter = std::make_shared<ZhuBridsonPointsToImplicit3>(data->kernelRadius(), sZhuBridsonCutOffThreshold, false);
	//converter = std::make_shared<SolenthalerPointsToImplicit3>(data->kernelRadius(), sZhuBridsonCutOffThreshold, false);
	converter = std::make_shared<AnisotropicPointsToImplicit3>(data->kernelRadius(), 0.5, 0.5, 25, false);


	VertexCenteredScalarGrid3 sdf(_resolution, Vector3D({ particles->radius(),particles->radius(),particles->radius() }),
		_origin);
	converter->convert(particles->positions(), &sdf);

	// convert to triangle mesh.
	TriangleMesh3 mesh;
	marchingCubes(sdf.constDataAccessor(), sdf.gridSpacing(), sdf.dataOrigin(), &mesh, 0.0, kDirectionAll);
	char basename[256];
	snprintf(basename, sizeof(basename), "frame_%06d.obj", frameCnt);
	std::string objName = std::string(basename);
	std::string filename = rootDir + objName;
	std::ofstream file(filename.c_str());
	if (file)
	{
		std::cout << "Writing " << filename << "...\n";
		mesh.writeObj(&file);
		file.close();
	}
	else
		std::cout << "Failed to save the file:" << filename << std::endl;

	// writing the xml file.
	snprintf(basename, sizeof(basename), "frame_%06d.xml", frameCnt);
	filename = rootDir + std::string(basename);
	std::ofstream xmlfile(filename.c_str());
	// calculate camera.
	double midX = (maxX + minX) / 2;
	double midY = minY + (maxY - minY) * 0.3;
	double midZ = (maxZ + minZ) / 2;
	double longest = std::max(std::max(maxX - minX, maxY - minY), maxZ - minZ) * 1.6;
	Vector3D target({ midX, midY, midZ });
	Vector3D origin({ 0.3,0.4,1 });
	origin.normalize();
	origin = origin * longest + target;
	if (xmlfile)
	{
		std::cout << "Writing " << filename << "...\n";
		xmlfile << "<scene version=\"0.5.0\">";
		xmlfile << "<integrator type=\"volpath_simple\">";
		xmlfile << "<integer name=\"maxDepth\" value=\"20\"/>";
		xmlfile << "</integrator>";
		xmlfile << "<sensor type=\"perspective\">";
		xmlfile << "<transform name=\"toWorld\">\n";
		xmlfile << "<lookat target=\"" << target.x << "," << target.y << "," << target.z
			<< "\" origin=\"" << origin.x << "," << origin.y << "," << origin.z
			<< "\" up=\"" << 0.0 << "," << 1.0 << "," << 0.0 << "\"/>\n";
		xmlfile << "</transform>";
		xmlfile << "<sampler type=\"ldsampler\">";
		xmlfile << "<integer name=\"sampleCount\" value=\"512\"/>";
		xmlfile << "</sampler>";
		xmlfile << "<film type=\"ldrfilm\">";
		xmlfile << "<integer name=\"width\" value=\"800\"/>";
		xmlfile << "<integer name=\"height\" value=\"600\"/>";
		xmlfile << "<string name=\"pixelFormat\" value=\"rgb\"/>";
		xmlfile << "</film>";
		xmlfile << "</sensor>";
		xmlfile << _sceneXml;
		xmlfile << "<emitter type=\"envmap\" id=\"envmapLight\">";
		xmlfile << "<string name=\"filename\" value=\"../envmap.exr\"/>";
		xmlfile << "<transform name=\"toWorld\">";
		xmlfile << "</transform>";
		xmlfile << "<float name=\"scale\" value=\"2.75\"/>";
		xmlfile << "</emitter>";
		xmlfile << "<shape type=\"obj\" id=\"water\">";
		xmlfile << "<string name=\"filename\" value=\"" << objName << "\"/>";
		xmlfile << "<bsdf type=\"dielectric\">";
		xmlfile << "<float name=\"intIOR\" value=\"1.33\"/>";
		xmlfile << "<float name=\"extIOR\" value=\"1.0\"/>";
		xmlfile << "</bsdf>";
		xmlfile << "<medium name=\"interior\" type=\"homogeneous\">";
		xmlfile << "<rgb name=\"sigmaS\" value=\"0, 0, 0\"/>";
		xmlfile << "<rgb name=\"sigmaA\" value=\"4, 1.7, 1.5\"/>";
		xmlfile << "</medium>";
		xmlfile << "</shape>";
		xmlfile << "</scene>";
		xmlfile.close();
	}
	else 
		std::cout << "Failed to save the file:" << filename << std::endl;

}

void ParticleFluidDemo::saveParticleAsXml(const Engine::ParticleSystemData3Ptr & particles,
	const std::string & rootDir, int frameCnt)
{
	Array1<Vector3D> positions(particles->numberOfParticles());
	copyRange1(particles->positions(), particles->numberOfParticles(), &positions);
	char basename[256];
	snprintf(basename, sizeof(basename), "frame_%06d.xml", frameCnt);
	std::string filename = rootDir + std::string(basename);
	std::ofstream file(filename.c_str());

	// calculate camera.
	double midX = (maxX + minX) / 2;
	double midY = minY + (maxY - minY) * 0.3;
	double midZ = (maxZ + minZ) / 2;
	double longest = std::max(std::max(maxX - minX, maxY - minY), maxZ - minZ) * 1.6;
	Vector3D target({ midX, midY, midZ });
	Vector3D origin({ -0.3,0.4,-1 });
	origin.normalize();
	origin = origin * longest + target;
		
	if (file)
	{
		std::cout << "Writing " << filename << "...\n";
		file << "<scene version=\"0.5.0\">";
		file << "<integrator type=\"path\">";
		file << "<integer name=\"maxDepth\" value=\"20\"/>";
		file << "</integrator>";
		file << "<sensor type=\"perspective\">";
		file << "<transform name=\"toWorld\">\n";
		file << "<lookat target=\"" << target.x << "," << target.y << "," << target.z
			<< "\" origin=\"" << origin.x << "," << origin.y << "," << origin.z
			<< "\" up=\"" << 0.0 << "," << 1.0 << "," << 0.0 << "\"/>\n";
		file << "</transform>";
		file << "<sampler type=\"ldsampler\">";
		file << "<integer name=\"sampleCount\" value=\"80\"/>";
		file << "</sampler>";
		file << "<film type=\"ldrfilm\">";
		file << "<integer name=\"width\" value=\"800\"/>";
		file << "<integer name=\"height\" value=\"600\"/>";
		file << "<string name=\"pixelFormat\" value=\"rgb\"/>";
		file << "</film>";
		file << "</sensor>";
		file << "<emitter type=\"envmap\" id=\"envmapLight\">";
		file << "<string name=\"filename\" value=\"../envmap.exr\"/>";
		file << "<transform name=\"toWorld\">";
		file << "</transform>";
		file << "<float name=\"scale\" value=\"2.75\"/>";
		file << "</emitter>";
		file << "<bsdf type=\"diffuse\" id=\"particleColor\">";
		file << "<rgb name=\"diffuseReflectance\" value=\".33, .84, .97\"/>";
		file << "</bsdf>";
		file << _sceneXml;
		file << "<shape type=\"shapegroup\" id=\"spheres\">";
		file << "<shape type=\"sphere\" id=\"particle\">";
		file << "<ref id=\"particleColor\"/>";
		file << "<float name=\"radius\" value=\"" << particles->radius()/2 << "\"/>";
		file << "</shape>";
		file << "</shape>";
		for (const auto &pos : positions)
		{
			file << "<shape type=\"instance\">";
			file << "<ref id=\"spheres\"/>";
			file << "<transform name=\"toWorld\">";

			char buffer[64];
			snprintf(buffer, sizeof(buffer), "<translate x=\"%f\" y=\"%f\" z=\"%f\"/>", pos.x, pos.y, pos.z);
			file << buffer;

			file << "</transform>";
			file << "</shape>";
		}
		file << "</scene>";
		file.close();
	}
	else
		std::cout << "Failed to save the file:" << filename << std::endl;
}

void ParticleFluidDemo::saveParticleAsPos(const Engine::ParticleSystemData3Ptr & particles,
	const std::string & rootDir, int frameCnt)
{
	Array1<Vector3D> positions(particles->numberOfParticles());
	copyRange1(particles->positions(), particles->numberOfParticles(), &positions);
	char basename[256];
	snprintf(basename, sizeof(basename), "frame_%06d.pos", frameCnt);
	std::string filename = rootDir + std::string(basename);
	std::ofstream file(filename.c_str(), std::ios::binary);
	if (file)
	{
		std::cout << "Writing " << filename << "...\n";
		std::vector<double> buffer;
		for (size_t i = 0; i < positions.size(); ++i)
		{
			buffer.push_back(positions[i].x);
			buffer.push_back(positions[i].y);
			buffer.push_back(positions[i].z);
		}
		file.write(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(double));
		file.close();
	}
	else
		std::cout << "Failed to save the file:" << filename << std::endl;
}

void ParticleFluidDemo::saveParticleAsXyz(const Engine::ParticleSystemData3Ptr & particles,
	const std::string & rootDir, int frameCnt)
{
	Array1<Vector3D> positions(particles->numberOfParticles());
	copyRange1(particles->positions(), particles->numberOfParticles(), &positions);
	const auto densities = reinterpret_cast<SphSystemData3*>(particles.get())->densities();
	char basename[256];
	snprintf(basename, sizeof(basename), "frame_%06d.xyz", frameCnt);
	std::string filename = rootDir + basename;
	std::ofstream file(filename.c_str());
	if(file)
	{
		std::cout << "Writing " << filename << "...\n";
		// bounding box.
		file << minX << ' ' << minY << ' ' << minZ << std::endl;
		file << maxX << ' ' << maxY << ' ' << maxZ << std::endl;
		// kernel radius.
		file << reinterpret_cast<SphSystemData3*>(particles.get())->kernelRadius() << std::endl;
		// particle radius.
		file << particles->radius() * 0.5 << std::endl;
		// particle mass.
		file << particles->mass() << std::endl;
		// particle positions.
		for (size_t i = 0; i < positions.size(); ++i)
			file << positions[i].x << ' ' << positions[i].y << ' ' << positions[i].z << ' ' << densities[i] << std::endl;
		//for (const auto &pt : positions)
		//	file << pt.x << ' ' << pt.y << ' ' << pt.z << std::endl;
		file.close();
	}
	else 
		std::cout << "Failed to save the file:" << filename << std::endl;

	char baseObjName[256];
	snprintf(baseObjName, sizeof(baseObjName), "frame_%06d.obj", frameCnt);
	std::string objName = std::string(baseObjName);

	// writing the xml file.
	snprintf(basename, sizeof(basename), "frame_%06d.xml", frameCnt);
	filename = rootDir + std::string(basename);
	std::ofstream xmlfile(filename.c_str());
	// calculate camera.
	double midX = (maxX + minX) / 2;
	double midY = minY + (maxY - minY) * 0.3;
	double midZ = (maxZ + minZ) / 2;
	double longest = std::max(std::max(maxX - minX, maxY - minY), maxZ - minZ) * 1.6;
	Vector3D target({ midX, midY, midZ });
	Vector3D origin({ 0.3,0.4,1 });
	origin.normalize();
	origin = origin * longest + target;
	if (xmlfile)
	{
		std::cout << "Writing " << filename << "...\n";
		xmlfile << "<scene version=\"0.5.0\">";
		xmlfile << "<integrator type=\"volpath_simple\">";
		xmlfile << "<integer name=\"maxDepth\" value=\"20\"/>";
		xmlfile << "</integrator>";
		xmlfile << "<sensor type=\"perspective\">";
		xmlfile << "<transform name=\"toWorld\">\n";
		xmlfile << "<lookat target=\"" << target.x << "," << target.y << "," << target.z
			<< "\" origin=\"" << origin.x << "," << origin.y << "," << origin.z
			<< "\" up=\"" << 0.0 << "," << 1.0 << "," << 0.0 << "\"/>\n";
		xmlfile << "</transform>";
		xmlfile << "<sampler type=\"ldsampler\">";
		xmlfile << "<integer name=\"sampleCount\" value=\"512\"/>";
		xmlfile << "</sampler>";
		xmlfile << "<film type=\"ldrfilm\">";
		xmlfile << "<integer name=\"width\" value=\"800\"/>";
		xmlfile << "<integer name=\"height\" value=\"600\"/>";
		xmlfile << "<string name=\"pixelFormat\" value=\"rgb\"/>";
		xmlfile << "</film>";
		xmlfile << "</sensor>";
		xmlfile << _sceneXml;
		xmlfile << "<emitter type=\"envmap\" id=\"envmapLight\">";
		xmlfile << "<string name=\"filename\" value=\"../envmap.exr\"/>";
		xmlfile << "<transform name=\"toWorld\">";
		xmlfile << "</transform>";
		xmlfile << "<float name=\"scale\" value=\"2.75\"/>";
		xmlfile << "</emitter>";
		xmlfile << "<shape type=\"obj\" id=\"water\">";
		xmlfile << "<string name=\"filename\" value=\"" << objName << "\"/>";
		xmlfile << "<bsdf type=\"dielectric\">";
		xmlfile << "<float name=\"intIOR\" value=\"1.33\"/>";
		xmlfile << "<float name=\"extIOR\" value=\"1.0\"/>";
		xmlfile << "</bsdf>";
		xmlfile << "<medium name=\"interior\" type=\"homogeneous\">";
		xmlfile << "<rgb name=\"sigmaS\" value=\"0, 0, 0\"/>";
		xmlfile << "<rgb name=\"sigmaA\" value=\"4, 1.7, 1.5\"/>";
		xmlfile << "</medium>";
		xmlfile << "</shape>";
		xmlfile << "</scene>";
		xmlfile.close();
	}
	else
		std::cout << "Failed to save the file:" << filename << std::endl;

}

void ParticleFluidDemo::readParticleFromFile(Array1<Vector3D>& positions,
	const std::string & rootDir, int frameCnt)
{
	char basename[256];
	snprintf(basename, sizeof(basename), "frame_%06d.xyz", frameCnt);
	std::string filename = rootDir + basename;
	std::ifstream file(filename.c_str());
	if (file)
	{
		std::string line;

		//! min point of bounding box.
		std::getline(file, line);

		//! max point of bounding box.
		std::getline(file, line);

		//! kernel radius.
		std::getline(file, line);

		//! particle radius.
		std::getline(file, line);

		//! particle mass.
		std::getline(file, line);

		//! particles' positions and densities.
		while (std::getline(file, line))
		{
			std::stringstream str;
			str << line;
			Vector3D tmp;
			str >> tmp.x;
			str >> tmp.y;
			str >> tmp.z;
			positions.append(tmp);
		}

		file.close();
	}
	else
		std::cout << "Failed to read the file:" << filename << std::endl;

	return;
}
