#pragma once
#include <vector>
#include <map>
namespace SE
{
	
	struct SNavVertex
	{
		Vector3f myPosition;
		int myIndex = -1;
	};

	struct SNavTriangle
	{
		unsigned int myIndex;
		SNavVertex myPoints[3];
		Vector3f myCentroid;
		std::vector<SNavTriangle*> myConnectedTriangles;
	};

	
	struct SNavMesh
	{
		std::vector<SNavTriangle*> myNavTriangles;
		std::vector<SNavVertex> myNavVertices;
	};

	class CNavMeshLoader
	{
	public:

		SNavMesh* LoadNavMesh(const std::string_view& aPath);

	private:

		void CalcTriangleCentroid(SE::SNavTriangle* navTriangle, Vector3f& centroid);
		std::vector<SNavVertex> myNavVertices;
	};
}

