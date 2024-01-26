#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include<unordered_map>

namespace SE
{
	class CModel
	{
		friend class CModelFactory;
	public:
		struct SMesh
		{
			enum ETextureMap {
				ETextureMap_Albedo,
				ETextureMap_Normal,
				ETextureMap_Material,
				ETextureMap_Count,
			};

			UINT myStride;
			UINT myOffset;
			UINT myNumberOfVertices;
			UINT myNumberOfIndices;

			//UnusedSpace<1, int> myGarbage;
			D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
			ID3D11InputLayout* myInputLayout;
			ID3D11Buffer* myVertexBuffer;
			ID3D11Buffer* myIndexBuffer;
			ID3D11VertexShader* myVertexShader;
			ID3D11PixelShader* myPixelShader;

			ID3D11ShaderResourceView* myTexture[3];
		};

		struct SSkeleton
		{
			struct SJoint
			{
				unsigned myIndex;
				char myName[32];
				Matrix4x4f myBindPoseInverse;
			};
			std::vector<SJoint> myJoints;
			std::unordered_map<std::string, int> myJointNameToIndex;
		};

		bool Init(const SMesh& aMesh);
		bool Init(std::vector<SMesh>& someMeshes);
		bool Init(const SMesh& aMesh, SSkeleton aSkeleton);
		bool Init(std::vector<SMesh>& someMeshes, SSkeleton aSkeleton);

		const std::vector<SMesh>& GetMeshes() const;
		const std::string& GetPath() const noexcept;
		void SetPath(const std::string& aPath);
		SSkeleton& GetSkeleton();
		bool HasBones() { return myHasBones; }
		float GetRadius() { return myRadius; }
	private:
		SSkeleton mySkeleton;
		bool myHasBones = false;
		float myRadius = 0;
		std::string myPath;
		std::vector<SMesh> myMeshes;
	};
}
