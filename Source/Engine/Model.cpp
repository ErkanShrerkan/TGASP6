#include "pch.h"
#include "Model.h"

namespace SE
{
	bool CModel::Init(const SMesh& aMesh, SSkeleton aSkeleton)
	{
		myMeshes.push_back(aMesh);
		mySkeleton = aSkeleton;
		myHasBones = true;
		return true;
	}

	bool CModel::Init(std::vector<SMesh>& someMeshes, SSkeleton aSkeleton)
	{
		myMeshes.swap(someMeshes);
		mySkeleton = aSkeleton;
		myHasBones = true;
		return true;
	}

	bool CModel::Init(const SMesh& aMesh)
	{
		myMeshes.push_back(aMesh);
		return true;
	}

	bool CModel::Init(std::vector<SMesh>& someMeshes)
	{
		myMeshes.swap(someMeshes);
		return true;
	}

	const std::vector<CModel::SMesh>& CModel::GetMeshes() const
	{
		return myMeshes;
	}

	const std::string& CModel::GetPath() const noexcept
	{
		return myPath;
	}

	void CModel::SetPath(const std::string& aPath)
	{
		myPath = aPath;
	}

	CModel::SSkeleton& CModel::GetSkeleton()
	{
		return mySkeleton;
	}
}
