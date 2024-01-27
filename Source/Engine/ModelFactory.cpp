#include "pch.h"
#include "ModelFactory.h"
#include <memory>

//#include <Windows.h>
#include <fstream>
#include "Engine.h"
#include "Model.h"
#include "Timer.h"
#include "Animator.h"

//#include "Texture.h"
#include "TextureHelper.h"
#include "ShaderHelper.h"

// The enum type '...' is unscoped. Prefer 'enum class' over 'enum' (Enum.3)
#pragma warning (push, 0)
#pragma warning (disable: 26812)
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <iostream>
#pragma warning (pop)

namespace SE
{
	CModel* CModelFactory::CreatePrimitive(const EPrimitive& aPrimitiveType)
	{
		switch (aPrimitiveType)
		{
		case EPrimitive::Cube:
		{
			return CreateCube();
		}
		default:
			assert(!"SE::CModelFactory::CreatePrimitive was fed an invalid SE::EPrimitive");
			return nullptr;
		}
	}

	CModel* CModelFactory::LoadFBX(const std::string& aPath)
	{
		Assimp::Importer importer;
		unsigned int flags;


		flags = 0
			| aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_SortByPType
			| aiProcess_MakeLeftHanded
			| aiProcess_FlipWindingOrder
			| aiProcess_FlipUVs
			// Caution, might lose data
			| aiProcess_OptimizeGraph
			| aiProcess_OptimizeMeshes;


		const aiScene* aiScene = importer.ReadFile(aPath.data(), flags);
		if (!aiScene)
		{
			perr("<%s> Could not load Model for \"%s\" fallback to \"%s\"",
				"SE::CModelFactory::LoadFBX",
				aPath.data(),
				"Models/Cube/Cube.fbx"
			);
			aiScene = importer.ReadFile("Models/Cube/Cube.fbx", 0
				| aiProcess_CalcTangentSpace
				| aiProcess_Triangulate
				| aiProcess_JoinIdenticalVertices
				| aiProcess_SortByPType
				| aiProcess_MakeLeftHanded
				| aiProcess_FlipWindingOrder
				| aiProcess_FlipUVs
				// Caution, might lose data
				| aiProcess_OptimizeGraph
				| aiProcess_OptimizeMeshes
			);
			if (!aiScene)
			{
				/* Error Message */
				return nullptr;
			}
		}
		pout("Model: \"%s\"", std::string(aPath).c_str());
		pout("Material Size: %u", aiScene->mNumMaterials);
		for (unsigned materialIndex = aiScene->mNumMaterials; materialIndex < aiScene->mNumMaterials; materialIndex++)
		{
			auto& material = aiScene->mMaterials[materialIndex];
			pout(" Material #%u: %s", materialIndex, material->GetName().C_Str());
			pout("  Properties: %u", material->mNumProperties);
			for (unsigned i = 0; i < material->mNumProperties; i++)
			{
				pout("   Property: \"%s\"", material->mProperties[i]->mKey.C_Str());
			}

			for (int i = 0; i < 18; i++)
			{
				for (unsigned textureIndex = 0; textureIndex < material->GetTextureCount(static_cast<aiTextureType>(i)); textureIndex++)
				{
					aiString path;
					material->GetTexture(static_cast<aiTextureType>(i), 0, &path);
					pout("  Texture #%u %i: %s", textureIndex, i, path.C_Str());
				}
			}
			/*for (unsigned textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE); textureIndex++)
			{
				aiString path;
				material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
				pout("  Texture #%u Diffuse: %s", textureIndex, path.C_Str());
			}
			for (unsigned textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType::aiTextureType_OPACITY); textureIndex++)
			{
				aiString path;
				material->GetTexture(aiTextureType::aiTextureType_OPACITY, 0, &path);
				pout("  Texture #%u Opacity: %s", textureIndex, path.C_Str());
			}*/
		}

		HRESULT result;
		auto* device = CEngine::GetInstance()->GetDXDevice();

		std::vector<CModel::SMesh> meshes;
		meshes.reserve(aiScene->mNumMeshes);
		for (unsigned meshIndex = 0; meshIndex < aiScene->mNumMeshes; meshIndex++)
		{
			auto& aiMesh = aiScene->mMeshes[meshIndex];

			/*unsigned materialIndex = aiMesh->mMaterialIndex;
			auto& material = aiScene->mMaterials[materialIndex];
			aiString path;
			if (material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE))
				material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);*/
				//pout("Material: %s\n", path.C_Str());

				/*std::string fixedPath = path.C_Str();
				{
					size_t index;
					while ((index = fixedPath.find("..")) != static_cast<size_t>(-1))
					{
						fixedPath.erase(index, 3);
					}
				}*/

				// Start Textures
			std::string name = std::string(aPath.substr(0, aPath.size() - 4));

			ID3D11ShaderResourceView* albedoSRV;
			Helper::TextureHelper::LoadShaderResourceView(&albedoSRV, name + "_c.dds");
			if (albedoSRV == nullptr)
			{
				perr("<%s> Could not load Albedo for \"%s\", fallback to \"%s\"",
					"SE::CModelFactory::CreateFBX",
					name.c_str(),
					"Textures/Error/Albedo_c.dds");

				Helper::TextureHelper::LoadShaderResourceView(&albedoSRV, "Textures/Error/Albedo_c.dds");
				if (albedoSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}

			ID3D11ShaderResourceView* normalSRV;
			Helper::TextureHelper::LoadShaderResourceView(&normalSRV, name + "_n.dds");
			if (normalSRV == nullptr)
			{
				perr("<%s> Could not load Albedo for \"%s\", fallback to \"%s\"",
					"SE::CModelFactory::CreateFBX",
					name.c_str(),
					"Textures/Error/Normal_n.dds");

				Helper::TextureHelper::LoadShaderResourceView(&normalSRV, "Textures/Error/Normal_n.dds");
				if (normalSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}

			ID3D11ShaderResourceView* materialSRV;
			Helper::TextureHelper::LoadShaderResourceView(&materialSRV, name + "_m.dds");
			if (materialSRV == nullptr)
			{
				perr("<%s> Could not load Material for \"%s\", fallback to \"%s\"",
					"SE::CModelFactory::CreateFBX",
					name.c_str(),
					"Textures/Error/Material_m.dds");

				Helper::TextureHelper::LoadShaderResourceView(&materialSRV, "Textures/Error/Material_m.dds");
				if (materialSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}
			// End Textures

			std::vector<SVertex> vertices;
			vertices.reserve(aiMesh->mNumVertices);
			for (unsigned vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; ++vertexIndex)
			{
				SVertex vertex = { 0 };
				/*if (aPath == "NavMesh/navmeshsquare.obj")
				{*/
				if (aPath == "Levels/StartingArea_Navmesh.obj")
				{
					/*if (aPath == "Levels/StartingArea.obj")
					{*/
					/*if (aPath == "NavMesh/navmeshJimmy.obj")
				{*/
					vertex.x = aiMesh->mVertices[vertexIndex].x * 1.f;
					vertex.y = aiMesh->mVertices[vertexIndex].y * 1.f;
					vertex.z = aiMesh->mVertices[vertexIndex].z * -1.f;
					vertex.w = 1.0f;
				}
				else
				{

					vertex.x = aiMesh->mVertices[vertexIndex].x * 1.f;
					vertex.y = aiMesh->mVertices[vertexIndex].y * 1.f;
					vertex.z = aiMesh->mVertices[vertexIndex].z * 1.f;
					vertex.w = 1.0f;

					//vertex.r = 1.f;//static_cast<float>(rand() % 255) / 255.f;
					//vertex.g = 1.f;//static_cast<float>(rand() % 255) / 255.f;
					//vertex.b = 1.f;//static_cast<float>(rand() % 255) / 255.f;

					if (aiMesh->HasNormals())
					{
						vertex.nx = aiMesh->mNormals[vertexIndex].x;
						vertex.ny = aiMesh->mNormals[vertexIndex].y;
						vertex.nz = aiMesh->mNormals[vertexIndex].z;
					}
					if (aiMesh->mTextureCoords[0])
					{
						vertex.u = aiMesh->mTextureCoords[0][vertexIndex].x;
						vertex.v = aiMesh->mTextureCoords[0][vertexIndex].y;
					}
					if (aiMesh->HasTangentsAndBitangents())
					{
						vertex.tx = aiMesh->mTangents[vertexIndex].x;
						vertex.ty = aiMesh->mTangents[vertexIndex].y;
						vertex.tz = aiMesh->mTangents[vertexIndex].z;

						vertex.bx = aiMesh->mBitangents[vertexIndex].x;
						vertex.by = aiMesh->mBitangents[vertexIndex].y;
						vertex.bz = aiMesh->mBitangents[vertexIndex].z;
					}
				}
				vertices.push_back(vertex);
			}

			std::vector<UINT> indices;
			indices.reserve(static_cast<size_t>(aiMesh->mNumFaces) * static_cast<size_t>(3u)); // Assume all faces have 3 triangles
			for (unsigned faceIndex = 0; faceIndex < aiMesh->mNumFaces; ++faceIndex)
			{
				aiFace& face = aiMesh->mFaces[faceIndex];
				for (unsigned indexIndex = 0; indexIndex < face.mNumIndices; ++indexIndex)
				{
					indices.emplace_back(face.mIndices[indexIndex]);
				}
			}
			indices.shrink_to_fit(); // Shrink so that we don't have more indices than we actually read

			D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
			vertexBufferDescription.ByteWidth = sizeof(SVertex) * static_cast<UINT>(vertices.size());
			vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
			vertexSubresourceData.pSysMem = vertices.data();

			ID3D11Buffer* vertexBuffer;
			result = device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
			if (FAILED(result))
			{
				/* Error Message */
				return nullptr;
			}

			D3D11_BUFFER_DESC indexBufferDescription = { 0 };
			indexBufferDescription.ByteWidth = sizeof(UINT) * static_cast<UINT>(indices.size());
			indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
			indexSubresourceData.pSysMem = indices.data();

			ID3D11Buffer* indexBuffer;
			result = device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
			if (FAILED(result))
			{
				/* Error Message */
				return nullptr;
			}
			// End Vertex

			// Start Shader
			std::string vsData;
			ID3D11VertexShader* vertexShader;
			if (!Helper::ShaderHelper::CreateVertexShader(&vertexShader, "Shaders/MeshVertexShader", &vsData)) { return nullptr; }

			ID3D11PixelShader* pixelShader;
			if (!Helper::ShaderHelper::CreatePixelShader(&pixelShader, "Shaders/MeshPixelShader")) { return nullptr; }
			// End Shader

			// Start Layout
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEIDS",     0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			ID3D11InputLayout* inputLayout;
			result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout);

			if (FAILED(result))
			{
				/* Error Message */
				return nullptr;
			}
			// End Layout

			CModel::SMesh mesh{};
			mesh.myNumberOfVertices = static_cast<UINT>(vertices.size());// sizeof(vertices) / sizeof(SVertex);
			mesh.myNumberOfIndices = static_cast<UINT>(indices.size());// sizeof(indices) / (sizeof(unsigned int));
			mesh.myStride = sizeof(SVertex);
			mesh.myOffset = 0;
			mesh.myVertexBuffer = vertexBuffer;
			mesh.myIndexBuffer = indexBuffer;
			mesh.myVertexShader = vertexShader;
			mesh.myPixelShader = pixelShader;
			mesh.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			mesh.myInputLayout = inputLayout;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Albedo] = albedoSRV;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Normal] = normalSRV;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Material] = materialSRV;
			meshes.push_back(mesh);
		}

		CModel* model = new CModel();
		model->Init(meshes);
		model->SetPath(aPath.data());
		return model;
	}

	CModel* CModelFactory::LoadNavMeshModel(const std::string& aPath)
	{
		Assimp::Importer importer;
		unsigned int flags;



		flags = 0
			| aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_SortByPType
			| aiProcess_MakeLeftHanded
			/*| aiProcess_FlipWindingOrder*/
			| aiProcess_FlipUVs
			// Caution, might lose data
			| aiProcess_OptimizeGraph
			| aiProcess_OptimizeMeshes;


		const aiScene* aiScene = importer.ReadFile(aPath.data(), flags);
		if (!aiScene)
		{
			perr("<%s> Could not load Model for \"%s\" fallback to \"%s\"",
				"SE::CModelFactory::CreateFBX",
				aPath.data(),
				"Models/Cube/Cube.fbx"
			);
			aiScene = importer.ReadFile("Models/Cube/Cube.fbx", 0
				| aiProcess_CalcTangentSpace
				| aiProcess_Triangulate
				| aiProcess_JoinIdenticalVertices
				| aiProcess_SortByPType
				| aiProcess_MakeLeftHanded
				| aiProcess_FlipWindingOrder
				| aiProcess_FlipUVs
				// Caution, might lose data
				| aiProcess_OptimizeGraph
				| aiProcess_OptimizeMeshes
			);
			if (!aiScene)
			{
				/* Error Message */
				return nullptr;
			}
		}
		pout("Model: \"%s\"", std::string(aPath).c_str());
		pout("Material Size: %u", aiScene->mNumMaterials);
		for (unsigned materialIndex = aiScene->mNumMaterials; materialIndex < aiScene->mNumMaterials; materialIndex++)
		{
			auto& material = aiScene->mMaterials[materialIndex];
			pout(" Material #%u: %s", materialIndex, material->GetName().C_Str());
			pout("  Properties: %u", material->mNumProperties);
			for (unsigned i = 0; i < material->mNumProperties; i++)
			{
				pout("   Property: \"%s\"", material->mProperties[i]->mKey.C_Str());
			}

			for (int i = 0; i < 18; i++)
			{
				for (unsigned textureIndex = 0; textureIndex < material->GetTextureCount(static_cast<aiTextureType>(i)); textureIndex++)
				{
					aiString path;
					material->GetTexture(static_cast<aiTextureType>(i), 0, &path);
					pout("  Texture #%u %i: %s", textureIndex, i, path.C_Str());
				}
			}
			/*for (unsigned textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE); textureIndex++)
			{
				aiString path;
				material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
				pout("  Texture #%u Diffuse: %s", textureIndex, path.C_Str());
			}
			for (unsigned textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType::aiTextureType_OPACITY); textureIndex++)
			{
				aiString path;
				material->GetTexture(aiTextureType::aiTextureType_OPACITY, 0, &path);
				pout("  Texture #%u Opacity: %s", textureIndex, path.C_Str());
			}*/
		}

		HRESULT result;
		auto* device = CEngine::GetInstance()->GetDXDevice();

		std::vector<CModel::SMesh> meshes;
		meshes.reserve(aiScene->mNumMeshes);
		for (unsigned meshIndex = 0; meshIndex < aiScene->mNumMeshes; meshIndex++)
		{
			auto& aiMesh = aiScene->mMeshes[meshIndex];

			/*unsigned materialIndex = aiMesh->mMaterialIndex;
			auto& material = aiScene->mMaterials[materialIndex];
			aiString path;
			if (material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE))
				material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);*/
				//pout("Material: %s\n", path.C_Str());

				/*std::string fixedPath = path.C_Str();
				{
					size_t index;
					while ((index = fixedPath.find("..")) != static_cast<size_t>(-1))
					{
						fixedPath.erase(index, 3);
					}
				}*/

				// Start Textures
			std::string name = std::string(aPath.substr(0, aPath.size() - 4));

			ID3D11ShaderResourceView* albedoSRV;
			Helper::TextureHelper::LoadShaderResourceView(&albedoSRV, name + "_c.dds");
			if (albedoSRV == nullptr)
			{
				perr("<%s> Could not load Albedo for \"%s\", fallback to \"%s\"",
					"SE::CModelFactory::CreateFBX",
					name.c_str(),
					"Textures/Error/Albedo_c.dds");

				Helper::TextureHelper::LoadShaderResourceView(&albedoSRV, "Textures/Error/Albedo_c.dds");
				if (albedoSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}

			ID3D11ShaderResourceView* normalSRV;
			Helper::TextureHelper::LoadShaderResourceView(&normalSRV, name + "_n.dds");
			if (normalSRV == nullptr)
			{
				perr("<%s> Could not load Albedo for \"%s\", fallback to \"%s\"",
					"SE::CModelFactory::CreateFBX",
					name.c_str(),
					"Textures/Error/Normal_n.dds");

				Helper::TextureHelper::LoadShaderResourceView(&normalSRV, "Textures/Error/Normal_n.dds");
				if (normalSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}

			ID3D11ShaderResourceView* materialSRV;
			Helper::TextureHelper::LoadShaderResourceView(&materialSRV, name + "_m.dds");
			if (materialSRV == nullptr)
			{
				perr("<%s> Could not load Material for \"%s\", fallback to \"%s\"",
					"SE::CModelFactory::CreateFBX",
					name.c_str(),
					"Textures/Error/Material_m.dds");

				Helper::TextureHelper::LoadShaderResourceView(&materialSRV, "Textures/Error/Material_m.dds");
				if (materialSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}
			// End Textures

			std::vector<SVertex> vertices;
			vertices.reserve(aiMesh->mNumVertices);
			for (unsigned vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; ++vertexIndex)
			{
				SVertex vertex = { 0 };

				vertex.x = aiMesh->mVertices[vertexIndex].x * 1.f;
				vertex.y = aiMesh->mVertices[vertexIndex].y * 1.f;
				vertex.z = aiMesh->mVertices[vertexIndex].z * -1.f;
				vertex.w = 1.0f;


				vertices.push_back(vertex);
			}

			std::vector<UINT> indices;
			indices.reserve(static_cast<size_t>(aiMesh->mNumFaces) * static_cast<size_t>(3u)); // Assume all faces have 3 triangles
			for (unsigned faceIndex = 0; faceIndex < aiMesh->mNumFaces; ++faceIndex)
			{
				aiFace& face = aiMesh->mFaces[faceIndex];
				for (unsigned indexIndex = 0; indexIndex < face.mNumIndices; ++indexIndex)
				{
					indices.emplace_back(face.mIndices[indexIndex]);
				}
			}
			indices.shrink_to_fit(); // Shrink so that we don't have more indices than we actually read

			D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
			vertexBufferDescription.ByteWidth = sizeof(SVertex) * static_cast<UINT>(vertices.size());
			vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
			vertexSubresourceData.pSysMem = vertices.data();

			ID3D11Buffer* vertexBuffer;
			result = device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
			if (FAILED(result))
			{
				/* Error Message */
				return nullptr;
			}

			D3D11_BUFFER_DESC indexBufferDescription = { 0 };
			indexBufferDescription.ByteWidth = sizeof(UINT) * static_cast<UINT>(indices.size());
			indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
			indexSubresourceData.pSysMem = indices.data();

			ID3D11Buffer* indexBuffer;
			result = device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
			if (FAILED(result))
			{
				/* Error Message */
				return nullptr;
			}
			// End Vertex

			// Start Shader
			std::string vsData;
			ID3D11VertexShader* vertexShader;
			if (!Helper::ShaderHelper::CreateVertexShader(&vertexShader, "Shaders/MeshVertexShader", &vsData)) { return nullptr; }

			ID3D11PixelShader* pixelShader;
			if (!Helper::ShaderHelper::CreatePixelShader(&pixelShader, "Shaders/MeshPixelShader")) { return nullptr; }
			// End Shader

			// Start Layout
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEIDS",     0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			ID3D11InputLayout* inputLayout;
			result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout);

			if (FAILED(result))
			{
				/* Error Message */
				return nullptr;
			}
			// End Layout

			CModel::SMesh mesh{};
			mesh.myNumberOfVertices = static_cast<UINT>(vertices.size());// sizeof(vertices) / sizeof(SVertex);
			mesh.myNumberOfIndices = static_cast<UINT>(indices.size());// sizeof(indices) / (sizeof(unsigned int));
			mesh.myStride = sizeof(SVertex);
			mesh.myOffset = 0;
			mesh.myVertexBuffer = vertexBuffer;
			mesh.myIndexBuffer = indexBuffer;
			mesh.myVertexShader = vertexShader;
			mesh.myPixelShader = pixelShader;
			mesh.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			mesh.myInputLayout = inputLayout;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Albedo] = albedoSRV;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Normal] = normalSRV;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Material] = materialSRV;
			meshes.push_back(mesh);
		}

		CModel* model = new CModel();
		model->Init(meshes);
		model->SetPath(aPath.data());
		return model;
	}

	CModel* CModelFactory::CreateCube()
	{
		HRESULT result;

		float halfUnit = 50.f;
		// Start Vertex
		SVertex vertices[8] = {
			/**| Position                           |**| Color               |**| UV          |**/
			{  halfUnit,  halfUnit,  halfUnit, 1.f, /**/ 1.f, 0.f, 0.f, 1.f, /**/ 0.f, 0.f }, // 1
			{  halfUnit,  halfUnit, -halfUnit, 1.f, /**/ 0.f, 1.f, 0.f, 1.f, /**/ 1.f, 0.f }, // 2
			{  halfUnit, -halfUnit,  halfUnit, 1.f, /**/ 0.f, 0.f, 1.f, 1.f, /**/ 0.f, 1.f }, // 3
			{ -halfUnit,  halfUnit,  halfUnit, 1.f, /**/ 0.f, 0.f, 0.f, 1.f, /**/ 1.f, 0.f }, // 4
			{  halfUnit, -halfUnit, -halfUnit, 1.f, /**/ 0.f, 1.f, 1.f, 1.f, /**/ 1.f, 1.f }, // 5
			{ -halfUnit,  halfUnit, -halfUnit, 1.f, /**/ 1.f, 0.f, 1.f, 1.f, /**/ 0.f, 0.f }, // 6
			{ -halfUnit, -halfUnit,  halfUnit, 1.f, /**/ 1.f, 1.f, 0.f, 1.f, /**/ 1.f, 1.f }, // 7
			{ -halfUnit, -halfUnit, -halfUnit, 1.f, /**/ 1.f, 1.f, 1.f, 1.f, /**/ 0.f, 1.f }  // 8
		};

		unsigned int indices[36] =
		{
			0, 2, 1,
			0, 1, 3,
			0, 3, 2,
			1, 2, 4,
			2, 3, 6,
			3, 1, 5,
			4, 5, 1,
			5, 6, 3,
			6, 4, 2,
			7, 6, 5,
			7, 5, 4,
			7, 4, 6
		};

		D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
		vertexBufferDescription.ByteWidth = sizeof(vertices);
		vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
		vertexSubresourceData.pSysMem = vertices;

		auto* device = CEngine::GetInstance()->GetDXDevice();
		ID3D11Buffer* vertexBuffer;
		result = device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
		if (FAILED(result))
		{
			/* Error Message */
			return nullptr;
		}

		D3D11_BUFFER_DESC indexBufferDescription = { 0 };
		indexBufferDescription.ByteWidth = sizeof(indices);
		indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
		indexSubresourceData.pSysMem = indices;

		ID3D11Buffer* indexBuffer;
		result = device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
		if (FAILED(result))
		{
			/* Error Message */
			return nullptr;
		}
		// End Vertex

		// Start Shader
		std::string vsData;
		ID3D11VertexShader* vertexShader;
		if (!Helper::ShaderHelper::CreateVertexShader(&vertexShader, "Shaders/UnlitVertexShader", &vsData)) { return nullptr; }

		ID3D11PixelShader* pixelShader;
		if (!Helper::ShaderHelper::CreatePixelShader(&pixelShader, "Shaders/UnlitPixelShader")) { return nullptr; }
		// End Shader

		// Start Layout
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		ID3D11InputLayout* inputLayout;
		result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout);

		if (FAILED(result))
		{
			/* Error Message */
			return nullptr;
		}
		// End Layout

		// Start Textures

		ID3D11ShaderResourceView* cubeSRV;
		Helper::TextureHelper::LoadShaderResourceView(&cubeSRV, "Textures/primitive_cube.dds");
		if (cubeSRV == nullptr)
		{
			/* Error Message */
			return nullptr;
		}
		// End Textures

		CModel* model = new CModel();
		if (!model)
		{
			/* Error Message */
			return nullptr;
		}

		CModel::SMesh mesh = {};
		mesh.myNumberOfVertices = sizeof(vertices) / sizeof(SVertex);
		mesh.myNumberOfIndices = sizeof(indices) / (sizeof(unsigned int));
		mesh.myStride = sizeof(SVertex);
		mesh.myOffset = 0;
		mesh.myVertexBuffer = vertexBuffer;
		mesh.myIndexBuffer = indexBuffer;
		mesh.myVertexShader = vertexShader;
		mesh.myPixelShader = pixelShader;
		mesh.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		mesh.myInputLayout = inputLayout;
		mesh.myTexture[0] = cubeSRV;
		model->Init(mesh);

		return model;
	}

#include <sys/stat.h>
#include <Engine\Animator.h>
	CModelFactory::~CModelFactory()
	{
		for (auto& [path, model] : myModels)
		{
			delete model;
		}

		myModels.clear();
	}

	CModel* CModelFactory::GetModel(const std::string& aPath)
	{
		struct stat buffer;
		if (stat(aPath.c_str(), &buffer) != 0)
		{
			return nullptr;
		}

		auto model = myModels.find(aPath);
		if (model != myModels.end())
		{
			return model->second;
		}

		std::string ext(aPath.end() - 4, aPath.end());
		if (ext == ".erc")
		{
			return LoadERC(aPath);
		}

		if (ext == ".fbx")
		{
			return LoadFBX(aPath);
		}

		return nullptr;
	}

	CModel* CModelFactory::LoadERC(const std::string& aPath)
	{
		CommonUtilities::Timer timer;
		printf("%s\n", aPath.c_str());
		timer.Update();

		//std::ifstream rf;
		//rf.open(aPath, std::ios::in | std::ios::binary);
		//std::string ercData = { std::istreambuf_iterator<char>(rf), std::istreambuf_iterator<char>() };
		//rf.close();

		std::ifstream t(aPath, std::ios::in | std::ios::binary);
		std::stringstream buffer;
		buffer << t.rdbuf();
		t.close();
		std::string ercData = buffer.str();

		timer.Update();
		printf("ercData load time: %f\n", timer.GetDeltaTime());

		auto device = CEngine::GetInstance()->GetDXDevice();

		int index = 0;
		int mMeshes = *reinterpret_cast<int*>(&ercData[0]);
		index += sizeof(int);
		float radius = *reinterpret_cast<float*>(&ercData[index]);
		index += sizeof(float);

		std::vector<CModel::SMesh> mMeshV;
		CModel::SSkeleton skeleton;

		bool hasBones = *reinterpret_cast<bool*>(&ercData[index]);
		index += sizeof(bool);

		if (hasBones)
		{
			int numJoints = *reinterpret_cast<int*>(&ercData[index]);
			index += sizeof(int);
			skeleton.myJoints.resize(numJoints);
			int size = sizeof(CModel::SSkeleton::SJoint) * numJoints;
			memcpy(&skeleton.myJoints[0], &ercData[index], size);
			index += size;
			int numNames = *reinterpret_cast<int*>(&ercData[index]);
			index += sizeof(int);
			const int nameSize = 32;
			for (int i = 0; i < numNames; i++)
			{
				char name[nameSize];
				memcpy(name, &ercData[index], nameSize);
				index += nameSize;
				int jointIndex = *reinterpret_cast<int*>(&ercData[index]);
				index += sizeof(int);
				skeleton.myJointNameToIndex[name] = jointIndex;
			}
		}
		timer.Update();
		printf("anim data load time: %f\n", timer.GetDeltaTime());

		for (size_t i = 0; i < mMeshes; i++)
		{
			HRESULT result;
			CModel::SMesh mesh;
			std::vector<SVertex> verts;
			std::vector<int> inds;

			int mNumVerts = *reinterpret_cast<int*>(&ercData[index]);
			index += sizeof(int);
			verts.resize(mNumVerts);
			int size = sizeof(SVertex) * mNumVerts;
			memcpy(&verts[0], &ercData[index], size);
			index += size;
			int mNumInds = *reinterpret_cast<int*>(&ercData[index]);
			index += sizeof(int);
			inds.resize(mNumInds);
			size = sizeof(int) * mNumInds;
			memcpy(&inds[0], &ercData[index], size);
			index += size;
			char mTexPath[128];
			size = sizeof(char) * 128;
			memcpy(&mTexPath[0], &ercData[index], size);
			index += size;

			std::string texture = std::string(mTexPath);
			texture = std::string(texture.begin(), texture.end());

			printf("Material name before: \"%s\"\n", texture.c_str());

			// Remove bogus character
			while (*(texture.end() - 1) < 32)
				texture = std::string(texture.begin(), texture.end() - 1);

			if (*(texture.end() - 1) == '_')
				texture = std::string(texture.begin(), texture.end() - 1);

			printf("Material name after: \"%s\"\n", texture.c_str());

			D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
			vertexBufferDescription.ByteWidth = sizeof(SVertex) * static_cast<UINT>(mNumVerts);
			vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
			vertexSubresourceData.pSysMem = verts.data();

			ID3D11Buffer* vertexBuffer;
			result = device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
			if (FAILED(result)) { return nullptr; }

			D3D11_BUFFER_DESC indexBufferDescription = { 0 };
			indexBufferDescription.ByteWidth = sizeof(UINT) * static_cast<UINT>(mNumInds);
			indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
			indexSubresourceData.pSysMem = inds.data();

			ID3D11Buffer* indexBuffer;
			result = device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
			if (FAILED(result)) { return nullptr; }

			std::string vsData;
			ID3D11VertexShader* vertexShader;
			if (!Helper::ShaderHelper::CreateVertexShader(&vertexShader, "Shaders/MeshVertexShader", &vsData)) { return nullptr; }

			ID3D11PixelShader* pixelShader;
			if (!Helper::ShaderHelper::CreatePixelShader(&pixelShader, "Shaders/MeshPixelShader")) { return nullptr; }
			// End Shader

			// Start Layout
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEIDS",     0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			ID3D11InputLayout* inputLayout;
			result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout);
			if (FAILED(result)) { return nullptr; }
			// End Layout

			// Start Textures
			std::string name = std::string(aPath.substr(0, aPath.size() - 4));

			std::string texturePath = "Models/";
			texturePath += texture;
			texturePath += "/";
			texturePath += texture;
			//std::string texturePath = "Models/CH_EY_BogScythe/CH_EY_BogScythe";

			ID3D11ShaderResourceView* albedoSRV;
			Helper::TextureHelper::LoadShaderResourceView(&albedoSRV, texturePath + "_c.dds");
			if (albedoSRV == nullptr)
			{
				perr("<%s> Could not load Albedo for \"%s\" (\"%s\"), fallback to \"%s\"",
					"SE::CModelFactory::LoadERC",
					name.c_str(),
					texturePath.c_str(),
					"Textures/Error/Albedo_c.dds");

				Helper::TextureHelper::LoadShaderResourceView(&albedoSRV, "Textures/Error/Albedo_c.dds");
				if (albedoSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}

			ID3D11ShaderResourceView* normalSRV;
			Helper::TextureHelper::LoadShaderResourceView(&normalSRV, texturePath + "_n.dds");
			if (normalSRV == nullptr)
			{
				//perr("<%s> Could not load Albedo for \"%s\", fallback to \"%s\"",
				//	"SE::CModelFactory::LoadERC",
				//	name.c_str(),
				//	"Textures/Error/Normal_n.dds");

				Helper::TextureHelper::LoadShaderResourceView(&normalSRV, "Textures/Error/Normal_n.dds");
				if (normalSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}

			ID3D11ShaderResourceView* materialSRV;
			Helper::TextureHelper::LoadShaderResourceView(&materialSRV, texturePath + "_m.dds");
			if (materialSRV == nullptr)
			{
				//perr("<%s> Could not load Material for \"%s\", fallback to \"%s\"",
				//	"SE::CModelFactory::LoadERC",
				//	name.c_str(),
				//	"Textures/Error/Material_m.dds");

				Helper::TextureHelper::LoadShaderResourceView(&materialSRV, "Textures/Error/Material_m.dds");
				if (materialSRV == nullptr)
				{
					/* Error Message */
					return nullptr;
				}
			}
			// End Textures

			mesh.myNumberOfVertices = static_cast<UINT>(mNumVerts);
			mesh.myNumberOfIndices = static_cast<UINT>(mNumInds);
			mesh.myStride = sizeof(SVertex);
			mesh.myOffset = 0;
			mesh.myVertexBuffer = vertexBuffer;
			mesh.myIndexBuffer = indexBuffer;
			mesh.myVertexShader = vertexShader;
			mesh.myPixelShader = pixelShader;
			mesh.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			mesh.myInputLayout = inputLayout;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Albedo] = albedoSRV;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Normal] = normalSRV;
			mesh.myTexture[CModel::SMesh::ETextureMap::ETextureMap_Material] = materialSRV;
			mMeshV.push_back(mesh);
		}
		timer.Update();
		printf("mesh data load time: %f\n", timer.GetDeltaTime());
		printf("total time: %f\n\n", timer.GetTotalTime());
		CModel* model = new CModel();
		if (hasBones)
		{
			model->Init(mMeshV, skeleton);
		}
		else
		{
			model->Init(mMeshV);
		}
		model->myRadius = radius;
		model->SetPath(aPath.data());
		//animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_Attack_AN.myr");
		myModels[aPath] = model;
		return model;
	}
}
