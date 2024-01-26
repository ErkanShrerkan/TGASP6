#include "pch.h"
#include "NavMeshLoader.h"

// The enum type '...' is unscoped. Prefer 'enum class' over 'enum' (Enum.3)
#pragma warning (push, 0)
#pragma warning (disable: 26812)
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#pragma warning (pop)
namespace SE
{
	SNavMesh* CNavMeshLoader::LoadNavMesh(const std::string_view& aPath)
	{
		Assimp::Importer importer;

		const aiScene* aiScene = importer.ReadFile(aPath.data(), 0
			| aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_SortByPType
			| aiProcess_MakeLeftHanded
			| aiProcess_FlipWindingOrder
			// Caution, might lose data
			| aiProcess_OptimizeGraph
			| aiProcess_OptimizeMeshes
		);
		std::vector<Vector3f> vertexPositions;
		SNavMesh* navMesh = new SNavMesh();
		for (unsigned int i = 0; i < aiScene->mMeshes[0]->mNumVertices; i++)
		{
			aiVector3D vertexPos = aiScene->mMeshes[0]->mVertices[i];
			SNavVertex navVertex;
			navVertex.myIndex = i;
			navVertex.myPosition = { vertexPos.x * 100.0f, vertexPos.y * 100.0f, vertexPos.z * -100.0f };
			navMesh->myNavVertices.push_back(navVertex);
		}
		for (unsigned int i = 0; i < aiScene->mMeshes[0]->mNumFaces; i++)
		{
			aiFace face = aiScene->mMeshes[0]->mFaces[i];
			SNavTriangle* navTriangle = new SNavTriangle();

			navTriangle->myPoints[0] = navMesh->myNavVertices[face.mIndices[2]];
			navTriangle->myPoints[1] = navMesh->myNavVertices[face.mIndices[1]];
			navTriangle->myPoints[2] = navMesh->myNavVertices[face.mIndices[0]];

			navTriangle->myIndex = i;

			Vector3f centroid;
			CalcTriangleCentroid(navTriangle, centroid);
			navTriangle->myCentroid = centroid;


			navMesh->myNavTriangles.push_back(navTriangle);
		}


		std::vector<SNavTriangle*> navTriangles = navMesh->myNavTriangles;
		for (size_t triangle1Idx = 0; triangle1Idx < navTriangles.size(); triangle1Idx++)
		{
			SNavTriangle* triangle1 = navTriangles[triangle1Idx];
			for (size_t triangle2Idx = 0; triangle2Idx < navTriangles.size(); triangle2Idx++)
			{
				bool triangleConnected = false;
				if (triangle1Idx == triangle2Idx)
				{
					continue;
				}
				SNavTriangle* triangle2 = navTriangles[triangle2Idx];
				unsigned int equalPoints = 0;
				for (size_t i = 0; i < 3; i++)
				{
					if (!triangleConnected)
					{
						for (size_t j = 0; j < 3; j++)
						{
							/*if (triangle1->myPoints[i].myIndex == triangle2->myPoints[j].myIndex)
							{
								equalPoints++;
							}*/
							//Some triangle points seem to have different indexes but be on (almost) the same position and should be considered as the same point.
							if ((triangle1->myPoints[i].myPosition - triangle2->myPoints[j].myPosition).LengthSqr() < 1.0f)
							{
								equalPoints++;
							}
						}
						if (equalPoints == 2)
						{
							bool triangleExistAlready = false;
							for (auto& connectedTriangle : triangle1->myConnectedTriangles)
							{
								if (connectedTriangle->myIndex == triangle2Idx)
								{
									triangleExistAlready = true;
								}
							}
							if (!triangleExistAlready)
							{

								triangle1->myConnectedTriangles.push_back(triangle2);

								triangleConnected = true;
								continue;
							}

						}
					}
				}
			}
		}

		return navMesh;

	}

	void CNavMeshLoader::CalcTriangleCentroid(SE::SNavTriangle* navTriangle, Vector3f& centroid)
	{
		float x = (navTriangle->myPoints[0].myPosition.x + navTriangle->myPoints[1].myPosition.x + navTriangle->myPoints[2].myPosition.x) / 3;
		float y = (navTriangle->myPoints[0].myPosition.y + navTriangle->myPoints[1].myPosition.y + navTriangle->myPoints[2].myPosition.y) / 3;
		float z = (navTriangle->myPoints[0].myPosition.z + navTriangle->myPoints[1].myPosition.z + navTriangle->myPoints[2].myPosition.z) / 3;
		centroid = { x,y,z };
	}

}
bool VectorsAreEqual(Vector3f firstVec, Vector3f secondVec)
{
	if (firstVec.x == secondVec.x &&
		firstVec.y == secondVec.y &&
		firstVec.z == secondVec.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}