#pragma once
#include "Engine/NavMeshLoader.h"
struct NavMesh
{
	std::vector<SE::SNavTriangle*> myNavTriangles;
	std::vector<SE::SNavVertex> myNavVertices;
};

