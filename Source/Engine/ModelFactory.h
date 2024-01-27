#pragma once
#include <string>
#include <CommonUtilities\Container\KeyedPool.hpp>

namespace SE
{
	enum class EPrimitive
	{
		Cube
	};

	class CModel;
	class CAnimator;
	class CModelFactory
	{
	public:
		~CModelFactory();
		CModel* GetModel(const std::string& aPath);
		CModel* CreatePrimitive(const EPrimitive& aPrimitiveType);
		CModel* LoadNavMeshModel(const std::string& aPath);
	private:
		CModel* LoadFBX(const std::string& aPath);
		CModel* CreateCube();
		CModel* LoadERC(const std::string& aPath);
		//CommonUtilities::KeyedPool<std::string, CModel> myPool;
		std::unordered_map<std::string, CModel*> myModels;
		struct SVertex
		{
			//SVertex() = default;
			//// Constructor for emplace_back purpose
			//SVertex(
			//	float aX, float aY, float aZ, float aW,
			//	float aR, float aG, float aB, float aA,
			//	float aU, float aV
			//) :
			//	x(aX), y(aY), z(aZ), w(aW),
			//	r(aR), g(aG), b(aB), a(aA),
			//	u(aU), v(aV)
			//{}

			float x, y, z, w;  // Position
			float r, g, b, a;  // Colors
			float u, v;		  // UV Mapping
			// TODO: Make block of 4 ???
			float nx, ny, nz;	  // Normal
			float tx, ty, tz;	  // Tangent
			float bx, by, bz;	  // Binormal

			unsigned ids[4];
			float weights[4];
		};
	};
}

