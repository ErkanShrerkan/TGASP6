#include "pch.h"
#include "PathFinding.h"
#include <vector>
#include <array>
#include <functional>
#include <cmath>
#include "CommonUtilities\Math\Vector.hpp"
#include <thread>


int SE::CPathFinding::GetLowestCurrDistIndex(std::map<SNavTriangle*, SNode>& someNodes)
{
	float min = FLT_MAX;
	int index = 0;

	for (auto& node : someNodes)
	{
		if (node.second.status == eStatus::eOpen && node.second.g <= min)
		{
			min = node.second.g;
			index = node.first->myIndex;
		}
	}

	return index;

}

int SE::CPathFinding::GetLowestNewCurrDistIndex(std::map<SNavTriangle*, SNewNode>& someNodes)
{
	int index = 0;

	for (auto& node : someNodes)
	{
		if (node.second.status == eStatus::eOpen && node.second.f > -1)
		{
			index = node.first->myIndex;
		}
	}

	return index;

}

bool SE::CPathFinding::IndexToBeChecked(int anIndex, std::vector<int>& aVector)
{
	for (auto v : aVector)
	{
		if (anIndex == v)
		{
			return true;
		}
	}

	return false;
}

bool SE::CPathFinding::HasOpenNodes(std::map<SNavTriangle*, SNode>& someNodes)
{

	//KAN OPTIMERAS
	for (auto& status : someNodes)
	{
		if (status.second.status == eStatus::eOpen)
		{
			return true;
		}
	}

	return false;
}

bool SE::CPathFinding::HasOpenNewNodes(std::map<SNavTriangle*, SNewNode>& someNodes)
{
	//KAN OPTIMERAS
	for (auto& status : someNodes)
	{
		if (status.second.status == eStatus::eOpen)
		{
			return true;
		}
	}

	return false;
}


std::vector<int> SE::CPathFinding::AStar(const std::vector<SNavTriangle*>& someNavTriangles, int aStartIndex, int anEndIndex, const bool& aShouldBeReversed)
{
	std::vector<int> resultPath;

	std::map<SNavTriangle*, SNode> nodes;

	std::map<int, std::vector<int>> adjacent;

	if (aStartIndex == anEndIndex)
	{
		resultPath.push_back(anEndIndex);
		return resultPath;
	}

	for (auto triangle : someNavTriangles)
	{
		SNode& node = nodes[triangle];
		node.g = FLT_MAX;
		node.status = eStatus::eUnvisited;

		Vector3f nodePos = triangle->myCentroid;

		Vector3f goalPos = someNavTriangles[anEndIndex]->myCentroid;

		float distance = (goalPos - nodePos).LengthSqr();
		node.h = distance;

	}

	nodes[someNavTriangles[aStartIndex]].g = 0;
	nodes[someNavTriangles[aStartIndex]].status = eStatus::eOpen;

	int v = 0;
	while (HasOpenNodes(nodes))
	{
		v = GetLowestCurrDistIndex(nodes);
		nodes[someNavTriangles[v]].status = eStatus::eClosed;

		if (v == anEndIndex)
		{
			break;
		}

		for (auto u : someNavTriangles[v]->myConnectedTriangles)
		{
			if (nodes[u].status != eStatus::eClosed)
			{
				nodes[u].status = eStatus::eOpen;

				if (nodes[u].g + nodes[u].h > nodes[someNavTriangles[v]].g + nodes[someNavTriangles[v]].h + 1)
				{
					nodes[u].g = nodes[someNavTriangles[v]].g + 1;
					nodes[u].predecessorIndex = v;
				}
			}
		}
	}

	if (nodes[someNavTriangles[v]].h > 2)
	{
		return resultPath;
	}

	v = someNavTriangles[anEndIndex]->myIndex;

	while (v != aStartIndex)
	{
		resultPath.push_back(v);
		try
		{
			v = nodes[someNavTriangles[v]].predecessorIndex;
		}
		catch (const std::exception&)
		{
			resultPath.clear();
			return resultPath;
		}
	}


	resultPath.push_back(aStartIndex);

	if (aShouldBeReversed)
	{
		std::reverse(resultPath.begin(), resultPath.end());
	}




	return resultPath;
}



std::vector<int> SE::CPathFinding::FloodFill(const std::vector<SNavTriangle*>& someNavTriangles, int aStartIndex, int anEndIndex,
	const bool& aShouldBeReversed, std::map<SE::SNavTriangle*, SNode>& someNodes)
{
	std::vector<int> resultPath;

	std::map<SNavTriangle*, SNode> nodes;

	std::map<int, std::vector<int>> adjacent;

	if (aStartIndex == anEndIndex)
	{
		resultPath.push_back(anEndIndex);
		return resultPath;
	}

	for (auto triangle : someNavTriangles)
	{
		SNode& node = nodes[triangle];
		node.g = FLT_MAX;
		node.status = eStatus::eUnvisited;

	}

	nodes[someNavTriangles[aStartIndex]].g = 0;
	nodes[someNavTriangles[aStartIndex]].status = eStatus::eOpen;

	int v = 0;
	while (HasOpenNodes(nodes))
	{
		v = GetLowestCurrDistIndex(nodes);
		nodes[someNavTriangles[v]].status = eStatus::eClosed;

		if (v == anEndIndex)
		{
			break;
		}

		for (auto u : someNavTriangles[v]->myConnectedTriangles)
		{
			if (nodes[u].status != eStatus::eClosed)
			{
				nodes[u].status = eStatus::eOpen;

				if (nodes[u].g + nodes[u].h > nodes[someNavTriangles[v]].g + nodes[someNavTriangles[v]].h + 1)
				{
					nodes[u].g = nodes[someNavTriangles[v]].g + 1;
					nodes[u].predecessorIndex = v;
				}
			}
		}
	}

	if (nodes[someNavTriangles[v]].h > 2)
	{
		return resultPath;
	}

	while (v != aStartIndex)
	{
		resultPath.push_back(v);
		try
		{
			v = nodes[someNavTriangles[v]].predecessorIndex;
		}
		catch (const std::exception&)
		{
			resultPath.clear();
			return resultPath;
		}
	}


	resultPath.push_back(aStartIndex);

	if (aShouldBeReversed)
	{
		std::reverse(resultPath.begin(), resultPath.end());
	}

	someNodes = nodes;


	return resultPath;
}


std::vector<int> SE::CPathFinding::BjornFill(const std::vector<SNavTriangle*>& someNavTriangles, int aPlayerIndex,
	std::map<SE::SNavTriangle*, SNewNode>& someNodes)
{
	if (someNodes.size() == 0)
	{
		for (auto triangle : someNavTriangles)
		{
			SNewNode& node = someNodes[triangle];
			node.f = -1;

		}
	}

	for (auto& node : someNodes)
	{
		if ((int)node.first->myIndex == aPlayerIndex)
		{
			node.second.f = 0;
			continue;
		}
		if (node.first->myConnectedTriangles.size() == 0)
		{
			continue;
		}
		int lowestDistance =-1;
		int size = (int)node.first->myConnectedTriangles.size();
		for (int i = 0; i < size; i++)
		{
			if (lowestDistance == -1)
			{
				lowestDistance = someNodes[node.first->myConnectedTriangles[i]].f;
				continue;
			}
				
			if (someNodes[node.first->myConnectedTriangles[i]].f != -1 && someNodes[node.first->myConnectedTriangles[i]].f < lowestDistance)
			{
				lowestDistance = someNodes[node.first->myConnectedTriangles[i]].f;
			}

		}
		node.second.f = lowestDistance + 1;

		//std::this_thread::sleep_for(std::chrono::nanoseconds(100));
	}

	std::vector<int> resultPath;


	return resultPath;
}

void SE::CPathFinding::FindClosestNeighbour(SE::SNavTriangle* currentTriangle, std::map<SE::SNavTriangle*, SNewNode>& someNodes, std::vector<int>& aIndexPath)
{
	
	int currentDist = someNodes[currentTriangle].f;
	int nearestNeighbourIndex = 0;
	bool neighbourNearerFound = false;
	if (someNodes[currentTriangle].f != 0)
	{
		for (int i = 0; i < currentTriangle->myConnectedTriangles.size(); i++)
		{
			if (someNodes[currentTriangle->myConnectedTriangles[i]].f < currentDist)
			{
				neighbourNearerFound = true;
				currentDist = someNodes[currentTriangle->myConnectedTriangles[i]].f;
				nearestNeighbourIndex = i;
			}
		}
	}
	/*else if (someNodes[currentTriangle].f == 0)
	{
		aIndexPath.push_back(currentTriangle->myIndex);
	}*/
	if (neighbourNearerFound)
	{
		aIndexPath.push_back(currentTriangle->myIndex);
		FindClosestNeighbour(currentTriangle->myConnectedTriangles[nearestNeighbourIndex], someNodes, aIndexPath);
	}


}

std::vector<int> SE::CPathFinding::BjornFind(const std::vector<SE::SNavTriangle*>& someNavTriangles, std::map<SE::SNavTriangle*, SNewNode>& someNodes, int aStartIndex, int anEndIndex, const bool& aShouldBeReversed)
{
	std::vector<int> resultPath;

	std::map<int, std::vector<int>> adjacent;

	//int currentDistance = -1;

	if (aStartIndex == anEndIndex)
	{
		resultPath.push_back(anEndIndex);
		return resultPath;
	}


	FindClosestNeighbour(someNavTriangles[aStartIndex], someNodes, resultPath);
	

	resultPath.push_back(anEndIndex);
   	if (aShouldBeReversed)
	{
		std::reverse(resultPath.begin(), resultPath.end());
	}
	//resultPath.push_back(aStartIndex);




	return resultPath;
	
}




double SE::CPathFinding::CalcTriangleArea(double x1, double y1, double x2, double y2, double x3, double y3)
{
	return abs(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0;
}


std::vector<Vector3f> SE::CPathFinding::Funnel(const std::vector<SE::SNavTriangle*>& someNavTriangles, std::vector<int> aIndexPath, Vector3f aStartPos, Vector3f anEndPos)
{


	std::vector<SE::SNavVertex> leftPortals;
	std::vector<SE::SNavVertex> rightPortals;
	if (aIndexPath.size() < 1)
	{
		//Is Inside goal triangle
	}

	SE::SNavVertex startPortal;
	startPortal.myPosition = aStartPos;
	leftPortals.push_back(startPortal);
	rightPortals.push_back(startPortal);

	for (int triangleIndex = (int)aIndexPath.size() - 1; triangleIndex > 0; triangleIndex--)
	{

		SE::SNavTriangle* currentTriangle = someNavTriangles[(int)aIndexPath[triangleIndex]];
		SE::SNavTriangle* nextTriangle = someNavTriangles[(int)aIndexPath[triangleIndex - 1]];

		if (currentTriangle->myConnectedTriangles.size() == 3)
		{
			continue;
		}
		int t = 0;
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{

				/*if (nextTriangle->myPoints[i].myIndex == currentTriangle->myPoints[j].myIndex)
				{*/
				//Some triangle points seem to have different indexes but be on (almost) the same position and should be considered as the same point.
				if ((nextTriangle->myPoints[i].myPosition - currentTriangle->myPoints[j].myPosition).LengthSqr() < 1.0f)
				{
					if (i == 0)
					{
						t += 1;
					}
					else if (i == 1)
					{
						t += 2;
					}
					else if (i == 2)
					{
						t += 3;
					}
				}
			}

		}

		SE::SNavVertex corner1 = currentTriangle->myPoints[0];
		SE::SNavVertex corner2 = currentTriangle->myPoints[1];
		SE::SNavVertex corner3 = currentTriangle->myPoints[2];


		int index1 = 0;
		int index2 = 0;

		if (t == 3)
		{
			index1 = 1;
			index2 = 0;
		}
		else if (t == 4)
		{
			index1 = 2;
			index2 = 0;
		}
		else if (t == 5)
		{
			index1 = 2;
			index2 = 1;
		}

		assert(index1 != -1);
		assert(index2 != -1);

		const Vector3f directionToNext = (nextTriangle->myCentroid - currentTriangle->myCentroid).GetNormalized();
		const Vector3f directionToNextNormal = Vector3f(directionToNext.z, directionToNext.y, -directionToNext.x);

		if (directionToNextNormal.Dot(nextTriangle->myPoints[index1].myPosition - currentTriangle->myCentroid) < 0.f)
		{
			leftPortals.push_back(nextTriangle->myPoints[index1]);
			rightPortals.push_back(nextTriangle->myPoints[index2]);
		}
		else
		{
			leftPortals.push_back(nextTriangle->myPoints[index2]);
			rightPortals.push_back(nextTriangle->myPoints[index1]);
		}
	}


	//sista i left och right portals, targetpos
	SE::SNavVertex endPortal;
	endPortal.myPosition = anEndPos;
	leftPortals.push_back(endPortal);
	rightPortals.push_back(endPortal);



	/*someDebugPortals->push_back(leftPortals);
	someDebugPortals->push_back(rightPortals);*/

	std::vector<Vector3f> goalVectors;

	stringPull(leftPortals, rightPortals, &goalVectors);


	return goalVectors;
}

inline void vcpy(float* dst, const float* src)
{
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
}

inline float triarea2(Vector3f a, Vector3f b, Vector3f c)
{
	const float ax = b.x - a.x;
	const float ay = b.z - a.z;
	const float bx = c.x - a.x;
	const float by = c.z - a.z;
	return bx * ay - ax * by;
}

inline bool vequal(const float* a, const float* b)
{
	static const float eq = 0.001f * 0.001f;
	Vector2f vectorA = { a[0],a[1] };
	Vector2f vectorB = { b[0],b[1] };
	float lengthSqr = (vectorA - vectorB).LengthSqr();
	return lengthSqr < eq;
}


void SE::CPathFinding::stringPull(std::vector<SE::SNavVertex> leftPortals, std::vector<SE::SNavVertex> rightPortals,
	std::vector<Vector3f>* pts)
{

	//std::vector portal left & right

	// Find straight path.
	int npts = 0;
	// Init scan state
	Vector3f portalApex, portalLeft, portalRight;

	int apexIndex = 0, leftIndex = 0, rightIndex = 0;
	portalApex = leftPortals[0].myPosition; // 
	portalLeft = leftPortals[0].myPosition;
	portalRight = rightPortals[0].myPosition;

	// Add start point.
	pts->push_back(portalApex);
	npts++;

	for (int i = 1; i < leftPortals.size(); ++i)
	{
		Vector3f left = leftPortals[i].myPosition; // Vector3
		Vector3f right = rightPortals[i].myPosition;

		// Update right vertex.
		if (triarea2(portalApex, portalRight, right) <= 0.0f)
		{
			if ((portalApex - portalRight).LengthSqr() <= 0.0f || triarea2(portalApex, portalLeft, right) > 0.0f)
			{
				// Tighten the funnel.
				portalRight = right;
				rightIndex = i;
			}
			else
			{
				// Right over left, insert left to path and restart scan from portal left point.
				pts->push_back(portalLeft);
				npts++;
				// Make current left the new apex.
				portalApex = portalLeft;
				apexIndex = leftIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}

		// Update left vertex.
		if (triarea2(portalApex, portalLeft, left) >= 0.0f)
		{
			if ((portalApex - portalLeft).LengthSqr() <= 0.0f || triarea2(portalApex, portalRight, left) < 0.0f)
			{
				// Tighten the funnel.
				portalLeft = left;
				leftIndex = i;
			}
			else
			{
				// Left over right, insert right to path and restart scan from portal right point.
				pts->push_back(portalRight);
				npts++;
				// Make current right the new apex.
				portalApex = portalRight;
				apexIndex = rightIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}
	}
	pts->push_back(leftPortals[leftPortals.size() - 1].myPosition);


	//return pts;
}

//
//// Start portal
//vcpy(&portals[nportals * 4 + 0], startPos);
//vcpy(&portals[nportals * 4 + 2], startPos);
//nportals++;
//// Portal between navmesh polygons
//for (int i = 0; i < path->npolys - 1; ++i)
//{
//	getPortalPoints(mesh, path->poly[i], path->poly[i + 1], &portals[nportals * 4 + 0], &portals[nportals * 4 + 2]);
//	nportals++;
//}
//// End portal
//vcpy(&portals[nportals * 4 + 0], endPos);
//vcpy(&portals[nportals * 4 + 2], endPos);
//nportals++;