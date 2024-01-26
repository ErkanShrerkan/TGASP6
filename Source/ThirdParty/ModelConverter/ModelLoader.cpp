#include "ModelLoader.h"
#pragma warning (push, 0)
#pragma warning (disable: 26812)
#pragma warning (disable: 26451)
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#pragma warning (pop)
#include <Windows.h>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <fbxsdk.h>
#include <memory>

bool ModelLoader::LoadFBXWithBones(std::string aPath, std::unordered_map<std::string, unsigned>& parentNodes)
{
	// Skeleton data if we have bones.
	Skeleton mdlSkeleton;
	// We need to know this for later processing.
	bool hasBones = false;

	// This should probably live in the class and be reused.
	// Saves on allocation and memory.
	// The FBX Manager handles various internal things for the rest of the
	// FBX SDK system when in use.
	FbxManager* myFBXManager = FbxManager::Create();
	FbxImporter* fbxImporter = FbxImporter::Create(myFBXManager, "");

	// Load the file and make sure it's a valid FBX file.
	assert(fbxImporter->Initialize(aPath.c_str(), -1, myFBXManager->GetIOSettings()));
	assert(fbxImporter->IsFBX());

	// Things in the FBX file live in a scene which we need to work with.
	FbxScene* fbxScene = FbxScene::Create(myFBXManager, "The Scene");

	// This fixes the winding, and converts properly.
	// Changes -Z forward to +Z forward.
	// Flips Z on Normals, Binormals and Tangents.
	// Changes UV 1,1 from top right to bottom right.
	FbxAxisSystem fbxAxisSystem(FbxAxisSystem::eDirectX);
	fbxAxisSystem.ConvertScene(fbxScene);

	// I use this to keep track of the bone ID and Weights per Control Point.
	// (What a control point is we'll get to in a minute).
	std::unordered_multimap<unsigned int, std::pair<size_t, float>> ControlPointWeights;

	// Try to import the FBX file into the scene we've specified.
	bool success = fbxImporter->Import(fbxScene);
	if (!success)
	{
		return false;
	}

	// First we triangulate the scene we've imported.
	// We always want to work with triangles whenever possible.
	FbxGeometryConverter fbxConverter(myFBXManager);
	fbxConverter.Triangulate(fbxScene, true);

	// The FBX file format is a giant hierarchy of nodes.
	// They can be anything that the software supports and
	// this means that they are not only meshes! Right now
	// we just need all the meshes.
	// Therefore we go through the scene and find all meshes.
	std::vector<FbxNode*> mdlMeshNodes;
	GatherFBXMeshes(fbxScene->GetRootNode(), mdlMeshNodes);

	// For animations we need skeleton data so look for a skeleton and gather
	// all the joints that make up said skeleton.
	hasBones = GatherSkeletonData(fbxScene->GetRootNode(), mdlSkeleton, 0, 0);

	//for (auto& bone : mdlSkeleton.myJoints)
	//{
	//	parentNodes[&bone.myName[0]] = bone.myParentIndex;
	//}
	return true;

	// And then we load all meshes.
	// 
	// CAVEAT: This should be split into separate Mesh class objects for real world applications.
	// Otherwise rendering might be weird. Or at least have an index buffer with offsets
	// and make sure you offset the vertices in the vxbuffer too.
	//
	//meshes.resize(mdlMeshNodes.size());
	std::vector<Mesh> meshes;
	for (FbxNode* mdlMeshNode : mdlMeshNodes)
	{
		// The vertices we'll be using.
		std::vector<Vertex> mdlVertices;

		// And our indices to these vertices.
		std::vector<unsigned int> mdlIndices;

		// Since this is from GatherFBXMeshes we know that it's an FbxMesh.
		// That means that this shouldn't result in nullptr.
		FbxMesh* fbxMesh = mdlMeshNode->GetMesh();

		// Check if we have bones. If we do we need to gather binding information.
		if (hasBones)
		{
			// Load the FBX root transform. This is something provided by the modeling software.
			// All we need to do is assemble it.
			const FbxVector4 fbxTranslation = mdlMeshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
			const FbxVector4 fbxRotation = mdlMeshNode->GetGeometricRotation(FbxNode::eSourcePivot);
			const FbxVector4 fbxScale = mdlMeshNode->GetGeometricScaling(FbxNode::eSourcePivot);
			const fbxsdk::FbxAMatrix rootTransform = FbxAMatrix(fbxTranslation, fbxRotation, fbxScale);

			// The FBX world has many different kinds of deformers. They can be skeleton bones but
			// they can also be several other things such as forces, effects, scripts, what not.
			for (int deformIdx = 0; deformIdx < fbxMesh->GetDeformerCount(); deformIdx++)
			{
				// We only care about Skins, i.e. the method that binds a mesh to a skeleton.
				FbxSkin* fbxSkin = reinterpret_cast<FbxSkin*>(fbxMesh->GetDeformer(deformIdx, FbxDeformer::eSkin));

				// So if it's not a skin keep looking. You probably want a warning here if a model
				// has a skeleton but it's not bound!
				if (!fbxSkin)
					continue;

				// The Skin deform is made up out of skin clusters. These are for all intents
				// and purposes the joints that affect that section of the skin.
				for (int clusterIdx = 0; clusterIdx < fbxSkin->GetClusterCount(); clusterIdx++)
				{
					// So we go through each cluster.
					FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIdx);

					// We get the transform of the cluster that was present at skin binding time.
					// This is the "resting pose" if you like.
					fbxsdk::FbxAMatrix meshBindTransform;
					fbxCluster->GetTransformMatrix(meshBindTransform);

					// We also need the link transform. In the case of a Skin it will be the
					// transform to go from local Joint Space to World Space.
					fbxsdk::FbxAMatrix linkTransform;
					fbxCluster->GetTransformLinkMatrix(linkTransform);

					// And finally assemble the Bind Pose Transform.
					// This is the default position of the vertices.
					fbxsdk::FbxAMatrix bindPoseInverseTransform = linkTransform.Inverse() * meshBindTransform * rootTransform;
					// Remember to do this or you will get very interesting results :).
					//bindPoseInverseTransform = bindPoseInverseTransform.Transpose();

					// The Link in the skin cluster is the actual joint.
					// Because we already collected all joints we should be able to find it using
					// the acceleration map in the Skeleton.
					size_t jointIndex = mdlSkeleton.myJointNameToIndex[fbxCluster->GetLink()->GetName()];
					// Store the bind pose on the joint so we can access it later.
					for (int i = 0; i < 16; i++)
					{
						mdlSkeleton.myJoints[jointIndex].myBindPoseInverse[i] = bindPoseInverseTransform[i / 4][i % 4];
					}
					//mdlSkeleton.myJoints[jointIndex].BindPoseInverse = DB::ConvertFBXMatrix(bindPoseInverseTransform);

					// Here comes some more control point stuff.
					// We need to collect all the control points that this skin cluster affects.
					// And for those we need to store which joint affects it and its weights.
					for (int i = 0; i < fbxCluster->GetControlPointIndicesCount(); i++)
					{
						unsigned int c = fbxCluster->GetControlPointIndices()[i];
						float w = static_cast<float>(fbxCluster->GetControlPointWeights()[i]);
						// This inserts into a multimap.
						// c - control point index.
						// jointIndex - self explanatory.
						// w - the weight for this joint.
						ControlPointWeights.insert({ c, { jointIndex, w } });
					}

					// While we're at it we can read FBX animation data here.
					// Because you'll probably end up reading files differently in your engine
					// this is important to think about later.
					// You probably don't want to read vertices and weights from animation files
					// and just bother with the transforms. I do this here because this is an
					// example and it's less for you to navigate.

					// TGA SA bake their FBX animations when they export. This means that the
					// animation data is stored _per frame_ instead. Essentially every frame
					// is a keyframe. This means that it's much easier to interact with than
					// the AssImp keys.

					// We'll load the first animation stack. Since we use Maya or Motion Builder it's
					// always 0.
				}
			}
		}

		// That is all for animation data! Now we have everything we need to animate the mesh!
		// Yay.
		// But we need to read the mesh data too!

		// If our mesh doesn't have any binormals or tangents we need to calculate them.
		if (fbxMesh->GetElementBinormalCount() == 0 || fbxMesh->GetElementTangentCount() == 0)
		{
			bool result = fbxMesh->GenerateTangentsData(0, true, false);
			if (!result)
			{
				return false;
			}
		}

		// FBX Meshes are stored with control points. A control point != vertex.
		// It is, instead, a bit more complicated, it's still a point in the mesh
		// but it only has location information.
		// 
		// A vertex is a point that has UVs, Normals, Tangents, etc.
		// To create a vertex we need the location of the control point and the
		// data that is required to create a specific vertex for a specific polygon.

		// To get this we need to read the mesh Per Vertex, Per Polygon.
		const int fbxMeshPolygonCount = fbxMesh->GetPolygonCount();
		for (int p = 0; p < fbxMeshPolygonCount; p++)
		{
			// Since we triangulated we know how many vertices we have per polygon.
			for (int v = 0; v < 3; v++)
			{
				// Get the vertex at this index in this polygon. This yields the
				// control point index we need to read data from.
				const int fbxControlPtIndex = fbxMesh->GetPolygonVertex(p, v);

				// And then get the control point that correspond to that polygon vertex.
				// The control point is just a position. Keep in mind that w=0 here!
				// That is not what it should be in our vectors.
				const FbxVector4 fbxVxPos = fbxMesh->GetControlPointAt(fbxControlPtIndex);

				// Load all the UV sets for this vertex. We support up to 4 of them.
				FbxVector2 fbxVxUVs[4];
				const int fbxNumUVs = fbxMesh->GetElementUVCount();
				// We need to know which UV index to read data from and we can easily
				// get that using the polygon and vertex of that polygon.
				const int fbxTextureUVIndex = fbxMesh->GetTextureUVIndex(p, v);
				for (int uv = 0; uv < fbxNumUVs && uv < 4; uv++)
				{
					FbxGeometryElementUV* fbxUvElement = fbxMesh->GetElementUV(uv);
					fbxVxUVs[uv] = fbxUvElement->GetDirectArray().GetAt(fbxTextureUVIndex);
				}

				// Loading normals and so on requires the same (p, v) indexes but there
				// are no functions that work like that. So instead we concatenate
				// the values.
				const int polygonIndex = p * 3 + v;

				// We get the normals for this polygon vertex.
				FbxVector4 fbxNormal;
				FbxGeometryElementNormal* fbxNormalElement = fbxMesh->GetElementNormal(0);
				auto mapMode = fbxNormalElement->GetMappingMode(); // eByPolygonVertex
				//assert(mapMode == 2);
				auto refMode = fbxNormalElement->GetReferenceMode(); // eDirect
				//assert(refMode == 0);
				fbxNormal = fbxNormalElement->GetDirectArray().GetAt(polygonIndex);

				// And the tangents.
				FbxVector4 fbxTangent;
				FbxGeometryElementTangent* fbxTangentElement = fbxMesh->GetElementTangent(0);
				fbxTangent = fbxTangentElement->GetDirectArray().GetAt(polygonIndex);

				// The Biormals
				FbxVector4 fbxBinormal;
				FbxGeometryElementBinormal* fbxBinormalElement = fbxMesh->GetElementBinormal(0);
				fbxBinormal = fbxBinormalElement->GetDirectArray().GetAt(polygonIndex);

				// And the vertex colors. We support 4 here too.
				FbxColor fbxColors[4];
				const int fbxNumVxColorChannels = fbxMesh->GetElementVertexColorCount();
				for (int col = 0; col < fbxNumVxColorChannels && col < 4; col++)
				{
					FbxGeometryElementVertexColor* colElement = fbxMesh->GetElementVertexColor(col);
					fbxColors[col] = colElement->GetDirectArray().GetAt(polygonIndex);
				}

				// During the animation load we created that multimap with ControlPoint to Weight data.
				// Now it's time to put that to use so we can store it in the vertices!                    
				unsigned int BoneIDs[] = { 0, 0, 0, 0 };
				float BoneWeights[] = { 0, 0, 0, 0 };
				// Get animation weight data
				if (hasBones)
				{
					// Since we're making a bit of a complex iteration we need to define the iterator.
					// It's a lot less to type that way.
					typedef std::unordered_multimap<unsigned int, std::pair<size_t, float>>::iterator MMIter;

					// Then we use equal range to get all the data stored for this specific control point.
					std::pair<MMIter, MMIter> values = ControlPointWeights.equal_range(fbxControlPtIndex);

					// This idx is to loop on the 4 indices of ID and Weight.
					int idx = 0;
					for (MMIter it = values.first; it != values.second && idx < 4; ++it)
					{
						std::pair<size_t, float> BoneAndWeight = it->second;
						BoneIDs[idx] = BoneAndWeight.first;
						BoneWeights[idx] = BoneAndWeight.second;
						idx++;
					}
				}

				Vertex vx;
				vx.x = static_cast<float>(fbxVxPos[0]);
				vx.y = static_cast<float>(fbxVxPos[1]);
				vx.z = static_cast<float>(fbxVxPos[2]);
				vx.w = 1;
				vx.r = static_cast<float>(fbxColors[0][0]);
				vx.g = static_cast<float>(fbxColors[0][1]);
				vx.b = static_cast<float>(fbxColors[0][2]);
				vx.a = static_cast<float>(fbxColors[0][3]);
				vx.u = static_cast<float>(fbxVxUVs[0][0]);
				vx.v = static_cast<float>(fbxVxUVs[0][1]);
				vx.nx = static_cast<float>(fbxNormal[0]);
				vx.ny = static_cast<float>(fbxNormal[1]);
				vx.nz = static_cast<float>(fbxNormal[2]);
				vx.tx = static_cast<float>(fbxTangent[0]);
				vx.ty = static_cast<float>(fbxTangent[1]);
				vx.tz = static_cast<float>(fbxTangent[2]);
				vx.bx = static_cast<float>(fbxBinormal[0]);
				vx.by = static_cast<float>(fbxBinormal[1]);
				vx.bz = static_cast<float>(fbxBinormal[2]);
				//vx.ids[0] = BoneIDs[0];
				//vx.ids[1] = BoneIDs[1];
				//vx.ids[2] = BoneIDs[2];
				//vx.ids[3] = BoneIDs[3];
				//vx.weights[0] = BoneWeights[0];
				//vx.weights[1] = BoneWeights[1];
				//vx.weights[2] = BoneWeights[2];
				//vx.weights[3] = BoneWeights[3];

				// A drawback of using control points is that we MAY get duplicate vertices.
				// This means we'll need to compare and ensure that it is a unique vert.
				// The only way to do that is to compare all data.
				bool alreadyExists = false;
				for (size_t i = 0; i < mdlVertices.size(); i++)
				{
					// This is Vertex::operator==. It runs a memcmp(A, B, size) == 0
					// which works well for Plain Old Data (POD) objects.
					if (mdlVertices[i] == vx)
					{
						// If the vertex already existed we just store its index.
						mdlIndices.push_back(static_cast<unsigned int>(i));
						alreadyExists = true;
						break;
					}
				}

				// If this is a new vertex we need to store it and its index.
				if (!alreadyExists)
				{
					mdlVertices.push_back(vx);
					mdlIndices.push_back(static_cast<unsigned int>(mdlVertices.size() - 1));
				}
			}
		}

		//auto material = mdlMeshNode->GetMaterial(mdlMeshNode->GetMaterialIndex(mdlMeshNode->GetName()));
		//const char* matName = material->GetName();

		// TODO use assimp to get material names

		//std::vector<std::string> mats = GetMaterials(aPath);
		int index = 0;
		for (int i = 0; i < mdlMeshNode->GetMaterialCount(); i++)
		{
			auto mat = mdlMeshNode->GetMaterial(i);

			for (int j = 0; j < fbxScene->GetMaterialCount(); j++)
			{
				if (mat == fbxScene->GetMaterial(j))
				{
					index = j;
				}
			}
		}

		Mesh mesh;
		mesh.myVertices = mdlVertices;
		mesh.myIndices = mdlIndices;
		mesh.myTexture = fbxScene->GetMaterial(index)->GetName();
		meshes.push_back(mesh);
	}

	auto slash = aPath.find_last_of('/');
	std::string modelName = aPath.substr(slash + 1, aPath.size());
	modelName = modelName.substr(0, modelName.find_last_of('.'));
	std::string folder = aPath.substr(0, slash) + "/";
	std::string filePath = folder + modelName + ".erc";

	UINT size = meshes.size();
	std::ofstream wf(filePath.c_str(), std::ios::out | std::ios::binary);
	wf.write((char*)&size, sizeof(UINT));
	wf.write((char*)&hasBones, sizeof(bool));

	int numJoints = mdlSkeleton.myJoints.size();
	wf.write((char*)&numJoints, sizeof(int));
	wf.write((char*)&mdlSkeleton.myJoints[0], sizeof(Skeleton::Joint) * numJoints);
	int numNames = mdlSkeleton.myJointNameToIndex.size();
	wf.write((char*)&numNames, sizeof(int));
	for (auto& jointName : mdlSkeleton.myJointNameToIndex)
	{
		wf.write((char*)&jointName.first[0], sizeof(char) * 32);
		wf.write((char*)&jointName.second, sizeof(int));
	}

	for (const auto& mesh : meshes)
	{
		int numVerts = mesh.myVertices.size();
		wf.write((char*)&numVerts, sizeof(int));
		wf.write((char*)&mesh.myVertices[0], sizeof(Vertex) * numVerts);
		int numInds = mesh.myIndices.size();
		wf.write((char*)&numInds, sizeof(int));
		wf.write((char*)&mesh.myIndices[0], sizeof(UINT) * numInds);
		char buffer[128];
		memset(buffer, 0, 128);
		int index = 0;
		for (int i = 0; i < mesh.myTexture.size(); i++)
		{
			buffer[i] = mesh.myTexture[i];
			index++;
		}
		for (int i = index + 1; i < (128 - index); i++)
		{
			buffer[i] = 0;
		}
		wf.write(&buffer[0], sizeof(char) * 128);
	}
	wf.close();

	return true;
}

bool ModelLoader::GatherSkeletonData(FbxNode* aNode, std::vector<FbxNode*>& outBones, unsigned anIndex, unsigned aParentIndex)
{
	if (aNode->GetNodeAttribute() && aNode->GetNodeAttribute()->GetAttributeType() && aNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		outBones.push_back(aNode);
	}

	for (int i = 0; i < aNode->GetChildCount(); i++)
	{
		GatherSkeletonData(aNode->GetChild(i), outBones, (unsigned)outBones.size(), anIndex);
	}

	return outBones.size() > 0;
}

std::string ModelLoader::GetRootJoint(std::unordered_map<std::string, std::vector<std::string>>& aJointHierarchy)
{
	//bool foundRoot = false;
	//std::string rootContestant = &myModel->GetSkeleton().myJoints[0];
	//CAnimation* anim = myAnimations[myCurrentAnim];
	//while (!foundRoot)
	//{
	//	bool nameFound = false;
	//	for (auto& jointNames : anim->GetJointHierarchy())
	//	{
	//		for (auto& jointName : jointNames.second)
	//		{
	//			nameFound = (jointName == rootContestant->myName);
	//
	//			if (nameFound)
	//			{
	//				rootContestant = &myModel->GetSkeleton().myJoints[myModel->GetSkeleton().myJointNameToIndex[jointNames.first]];
	//				break;
	//			}
	//		}
	//	}
	//
	//	if (!nameFound)
	//	{
	//		foundRoot = true;
	//	}
	//}
	//return rootContestant;
	return "";
}

bool ModelLoader::LoadAnimation(void* aScene, std::string aPath)
{
	myNodeTransforms.clear();
	Assimp::Importer importer;

	unsigned flags =
		aiProcessPreset_TargetRealtime_MaxQuality |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_SortByPType;
	flags &= ~aiProcess_JoinIdenticalVertices;

	const aiScene* scene = importer.ReadFile(aPath.data(), flags);

	std::string animName = scene->mAnimations[0]->mName.C_Str();

	ModelLoader::Animation anim;
	float fps = scene->mAnimations[0]->mTicksPerSecond;
	float frames = scene->mAnimations[0]->mDuration;
	anim.SetDurationAndFPS(frames, fps);

	std::unordered_map<std::string, std::vector<std::string>> jointHierarchy;
	std::string rootName = scene->mRootNode->mName.C_Str();

	aiNode* node = scene->mRootNode;

	auto worldTransform = scene->mRootNode->mTransformation;
	float rootNodeTransform[16];
	worldTransform = worldTransform.Inverse().Transpose();
	memcpy(&rootNodeTransform[0], &(worldTransform.a1), sizeof(float) * 16);

	ReadNodeHierarchy((void*)scene, scene->mRootNode, jointHierarchy, scene->mRootNode->mName.C_Str(), true);
	myNodeTransforms;

	for (int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
	{
		std::string nodeName = scene->mAnimations[0]->mChannels[i]->mNodeName.C_Str();
		int nodeType = 0;

		if (nodeName.find_first_of('$') != std::string::npos)
		{
			std::string type = nodeName.substr(nodeName.find_first_of('$') - 1, nodeName.size());
			std::string baseName = nodeName.substr(0, nodeName.find_first_of('$') - 1);

			if (type == "_$AssimpFbx$_Translation")
			{
				nodeName = baseName + "$t";
			}
			else if (type == "_$AssimpFbx$_Rotation")
			{
				nodeName = baseName + "$r";
			}
			else if (type == "_$AssimpFbx$_Scaling")
			{
				nodeName = baseName + "$s";
			}
		}

		anim.AddJoint(nodeName);
		ModelLoader::Animation::Track& track = anim.GetTrack(nodeName);

		for (int n = 0; n < scene->mAnimations[0]->mChannels[i]->mNumPositionKeys; n++)
		{
			auto channel = scene->mAnimations[0]->mChannels[i];
			auto pos = scene->mAnimations[0]->mChannels[i]->mPositionKeys[n].mValue;
			auto time = scene->mAnimations[0]->mChannels[i]->mPositionKeys[n].mTime;
			ModelLoader::Animation::TranslationKey tk;
			tk.x = pos.x;
			tk.y = pos.y;
			tk.z = pos.z;
			tk.w = 1;
			tk.time = time;
			track.myTranslationKeys.push_back(tk);
		}
		for (int n = 0; n < scene->mAnimations[0]->mChannels[i]->mNumRotationKeys; n++)
		{
			auto rot = scene->mAnimations[0]->mChannels[i]->mRotationKeys[n].mValue;
			auto time = scene->mAnimations[0]->mChannels[i]->mRotationKeys[n].mTime;
			ModelLoader::Animation::RotationKey rk;
			rk.x = rot.x;
			rk.y = rot.y;
			rk.z = rot.z;
			rk.w = rot.w;
			rk.time = time;
			track.myRotationKeys.push_back(rk);
		}
		for (int n = 0; n < scene->mAnimations[0]->mChannels[i]->mNumScalingKeys; n++)
		{
			auto scl = scene->mAnimations[0]->mChannels[i]->mScalingKeys[n].mValue;
			auto time = scene->mAnimations[0]->mChannels[i]->mScalingKeys[n].mTime;
			ModelLoader::Animation::ScaleKey sk;
			sk.x = scl.x;
			sk.y = scl.y;
			sk.z = scl.z;
			sk.time = time;
			track.myScaleKeys.push_back(sk);
		}
	}

	auto slash = aPath.find_last_of('/');
	std::string modelName = aPath.substr(slash + 1, aPath.size());
	modelName = modelName.substr(0, modelName.find_last_of('.'));
	std::string folder = aPath.substr(0, slash) + "/";
	std::string filePath = folder + modelName + ".myr";

	std::ofstream wf(filePath.c_str(), std::ios::out | std::ios::binary);

	wf.write((char*)&rootNodeTransform[0], sizeof(float) * 16);
	UINT size = jointHierarchy.size();
	wf.write((char*)&size, sizeof(UINT));
	wf.write(rootName.c_str(), 32);
	for (auto& joint : jointHierarchy)
	{
		wf.write(joint.first.c_str(), 32);
		unsigned numChildren = joint.second.size();
		wf.write((char*)&numChildren, sizeof(unsigned));
		for (auto& child : joint.second)
		{
			wf.write(child.c_str(), 32);
		}
	}

	size = myNodeTransforms.size();
	wf.write((char*)&size, sizeof(UINT));
	for (auto& node : myNodeTransforms)
	{
		wf.write(node.first.c_str(), 32);
		wf.write((char*)&node.second.matrix[0], sizeof(float) * 16);
	}

	size = anim.GetTracks().size();
	wf.write((char*)&anim.myDurationInFrames, sizeof(float) * 3);
	wf.write((char*)&size, sizeof(UINT));

	for (auto& track : anim.GetTracks())
	{
		wf.write(track.first.c_str(), 32);

		int size = track.second.myTranslationKeys.size();
		wf.write((char*)&size, sizeof(int));
		wf.write((char*)&track.second.myTranslationKeys[0], sizeof(ModelLoader::Animation::TranslationKey) * size);

		size = track.second.myRotationKeys.size();
		wf.write((char*)&size, sizeof(int));
		wf.write((char*)&track.second.myRotationKeys[0], sizeof(ModelLoader::Animation::RotationKey) * size);

		size = track.second.myScaleKeys.size();
		wf.write((char*)&size, sizeof(int));
		wf.write((char*)&track.second.myScaleKeys[0], sizeof(ModelLoader::Animation::ScaleKey) * size);
	}
	wf.close();

	return true;

	//FbxManager* myFBXManager = FbxManager::Create();
	//FbxImporter* fbxImporter = FbxImporter::Create(myFBXManager, "");
	//
	//assert(fbxImporter->Initialize(aPath.c_str(), -1, myFBXManager->GetIOSettings()));
	//assert(fbxImporter->IsFBX());
	//
	//FbxScene* fbxScene = FbxScene::Create(myFBXManager, "The Scene");
	//
	//FbxAxisSystem fbxAxisSystem(FbxAxisSystem::eDirectX);
	//fbxAxisSystem.ConvertScene(fbxScene);
	//
	//int numStacks = fbxScene->GetSrcObjectCount<FbxAnimStack>();
	//FbxAnimStack* fbxAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//
	//if (fbxAnimStack == nullptr)
	//{
	//	return false;
	//}
	//
	//FbxString animStackName = fbxAnimStack->GetName();
	//std::string animationName = animStackName.Buffer();
	//
	//// Then we load the Take for this animation. This can only be a single one in Maya
	//// but Motion Builder can have several! There's no standard at TGA saying we should
	//// support multiple right now, though. Check with the animators!
	//FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(animStackName);
	//
	//std::vector<FbxNode*> bones;
	//GatherSkeletonData(fbxScene->GetRootNode(), bones, 0, 0);
	//
	//const FbxVector4 fbxTranslation = fbxScene->GetRootNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
	//const FbxVector4 fbxRotation = fbxScene->GetRootNode()->GetGeometricRotation(FbxNode::eSourcePivot);
	//const FbxVector4 fbxScale = fbxScene->GetRootNode()->GetGeometricScaling(FbxNode::eSourcePivot);
	//const fbxsdk::FbxAMatrix rootTransform = FbxAMatrix(fbxTranslation, fbxRotation, fbxScale);
	//
	//bool hasBones;
	//if (fbxTakeInfo)
	//{
	//	// We need the start of the animation take.
	//	FbxTime fbxAnimStart = fbxTakeInfo->mLocalTimeSpan.GetStart();
	//	// And the end as well.
	//	FbxTime fbxAnimEnd = fbxTakeInfo->mLocalTimeSpan.GetStop();
	//
	//	// You can find out the duration of this animation in frames by asking the SDK to convert for you.
	//	// I convert to 24 FPS animation here but there are several different ones you can use.
	//	// Remember to +1 since we want the final frame too!
	//	FbxLongLong animTime = fbxAnimEnd.GetFrameCount(FbxTime::eFrames24) - fbxAnimStart.GetFrameCount(FbxTime::eFrames24) + 1;
	//
	//	// I store my animation directly in the joints. This is also not really that
	//	// useful in a production environment such as your game engines. It just
	//	// makes this easier to implement and look at! :).
	//	// It also doesn't cover the VG part of having blended animations.
	//
	//	Animation anim;
	//	Skeleton mdlSkeleton;
	//
	//	hasBones = GatherSkeletonData(fbxScene->GetRootNode(), mdlSkeleton, 0, 0);
	//	//////////////////////REMEMBER/////////////////////////////////
	//	//mdlSkeleton.myJoints[jointIndex].myAnim.resize(animTime + 1);
	//	anim.myAnimation.resize(animTime + 1);
	//	///////////////////////////////////////////////////////////////
	//
	//
	//	// Regardless we step through the animation frames and evaluate the animation transforms for each frame.
	//	for (FbxLongLong t = fbxAnimStart.GetFrameCount(FbxTime::eFrames24); t <= fbxAnimEnd.GetFrameCount(FbxTime::eFrames24); t++)
	//	{
	//		for (int i = 0; i < mdlSkeleton.myJoints.size(); i++)
	//		{
	//			anim.myAnimation[t].push_back(Skeleton::Joint());
	//		}
	//
	//		//for (auto node : bones)
	//		//{
	//		//	FbxTime currentTime;
	//		//	currentTime.SetFrame(t, FbxTime::eFrames24);
	//		//
	//		//	// We evaluate the joint transform for this keyframe.
	//		//	FbxAMatrix nodeTransform = node->EvaluateGlobalTransform(currentTime) * rootTransform;
	//		//	FbxAMatrix frameTransform = nodeTransform.Inverse() * fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
	//		//
	//		//	// Remember to transpose it.
	//		//	frameTransform = frameTransform.Transpose();
	//		//
	//		//	// Then store the resulting animation transform for this particular frame in the joint that owns it.
	//		//
	//		//	//////////////////////REMEMBER/////////////////////////////////
	//		//	for (int i = 0; i < 16; i++)
	//		//	{
	//		//		anim.myAnimation[t][i].myBindPoseInverse[i] = bindPoseInverseTransform[i / 4][i % 4];
	//		//	}
	//		//	///////////////////////////////////////////////////////////////
	//		//}
	//	}
	//}
}

bool ModelLoader::DeleteFileML(std::string aPath)
{
	return (remove(aPath.c_str()) == 0);
}

void* ModelLoader::FindAnimNode(void* anAnimation, std::string aName)
{
	aiAnimation* anim = (aiAnimation*)anAnimation;

	for (unsigned i = 0; i < anim->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = anim->mChannels[i];
		std::string nodeName(pNodeAnim->mNodeName.data);

		if (nodeName == aName)
		{
			return (void*)pNodeAnim;
		}
	}

	return nullptr;
}

void ModelLoader::ReadNodeHierarchy(void* aScene, void* aNode, std::unordered_map<std::string, std::vector<std::string>>& aNodeHierarchy, std::string aParentName, bool aParentWasNull)
{
	aiNode* node = (aiNode*)aNode;

	std::string nodeName(node->mName.data);

	Matrix transform;
	auto matrix = node->mTransformation;
	matrix = matrix.Transpose();
	memcpy(&transform.matrix[0], &(matrix.a1), sizeof(float) * 16);

	aiAnimation* anim = ((aiScene*)aScene)->mAnimations[0];
	aiNodeAnim* nodeAnim = (aiNodeAnim*)FindAnimNode(anim, nodeName);

	if (!aParentWasNull)
	{
		std::string newNodeName = nodeName;
		std::string newParentName = aParentName;
		if (nodeName.find_first_of('$') != std::string::npos)
		{
			std::string type = nodeName.substr(nodeName.find_first_of('$') - 1, nodeName.size());
			std::string baseName = nodeName.substr(0, nodeName.find_first_of('$') - 1);

			if (type == "_$AssimpFbx$_Translation")
			{
				newNodeName = baseName + "$t";
			}
			else if (type == "_$AssimpFbx$_Rotation")
			{
				newNodeName = baseName + "$r";
			}
			else if (type == "_$AssimpFbx$_Scaling")
			{
				newNodeName = baseName + "$s";
			}
			else if (type == "_$AssimpFbx$_PreRotation")
			{
				newNodeName = baseName + "$p";
			}
			else if (type == "_$AssimpFbx$_RotationPivot")
			{
				newNodeName = baseName + "$rp";
			}
			else if (type == "_$AssimpFbx$_RotationPivotInverse")
			{
				newNodeName = baseName + "$rpi";
			}
			else if (type == "_$AssimpFbx$_ScalingPivot")
			{
				newNodeName = baseName + "$sp";
			}
			else if (type == "_$AssimpFbx$_ScalingPivotInverse")
			{
				newNodeName = baseName + "$spi";
			}
		}
		if (newParentName.find_first_of('$') != std::string::npos)
		{
			std::string type = aParentName.substr(aParentName.find_first_of('$') - 1, aParentName.size());
			std::string baseName = aParentName.substr(0, aParentName.find_first_of('$') - 1);

			if (type == "_$AssimpFbx$_Translation")
			{
				newParentName = baseName + "$t";
			}
			else if (type == "_$AssimpFbx$_Rotation")
			{
				newParentName = baseName + "$r";
			}
			else if (type == "_$AssimpFbx$_Scaling")
			{
				newParentName = baseName + "$s";
			}
			else if (type == "_$AssimpFbx$_PreRotation")
			{
				newParentName = baseName + "$p";
			}
			else if (type == "_$AssimpFbx$_RotationPivot")
			{
				newParentName = baseName + "$rp";
			}
			else if (type == "_$AssimpFbx$_RotationPivotInverse")
			{
				newParentName = baseName + "$rpi";
			}
			else if (type == "_$AssimpFbx$_ScalingPivot")
			{
				newParentName = baseName + "$sp";
			}
			else if (type == "_$AssimpFbx$_ScalingPivotInverse")
			{
				newParentName = baseName + "$spi";
			}
		}

		myNodeTransforms[newNodeName] = transform;
		aNodeHierarchy[newParentName].push_back(newNodeName);
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ReadNodeHierarchy(aScene, node->mChildren[i], aNodeHierarchy, nodeName, false);
	}
}

bool ModelLoader::GatherSkeletonData(FbxNode* aNode, Skeleton& aSkeleton, unsigned anIndex, unsigned aParentIndex)
{
	if (aNode->GetNodeAttribute() && aNode->GetNodeAttribute()->GetAttributeType() && aNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Skeleton::Joint joint;
		joint.myIndex = aParentIndex;
		_memccpy(&joint.myName[0], aNode->GetName(), '\0', 32);
		aSkeleton.myJoints.push_back(joint);
		aSkeleton.myJointNameToIndex.insert({ &joint.myName[0], aSkeleton.myJoints.size() - 1 });
	}

	for (int i = 0; i < aNode->GetChildCount(); i++)
	{
		GatherSkeletonData(aNode->GetChild(i), aSkeleton, (unsigned)aSkeleton.myJoints.size(), anIndex);
	}

	return aSkeleton.myJoints.size() > 0;
}

void ModelLoader::GatherFBXMeshes(FbxNode* aNode, std::vector<FbxNode*>& outMeshes)
{
	for (int i = 0; i < aNode->GetChildCount(); i++)
	{
		FbxNode* childNode = aNode->GetChild(i);
		FbxNodeAttribute* childAttribute = childNode->GetNodeAttribute();
		if (!childAttribute)
		{
			GatherFBXMeshes(childNode, outMeshes);
		}
		else
		{
			FbxNodeAttribute::EType childAttributeType = childAttribute->GetAttributeType();
			if (childAttributeType != FbxNodeAttribute::eMesh)
			{
				GatherFBXMeshes(childNode, outMeshes);
			}
			else
			{
				outMeshes.push_back(childNode);
				GatherFBXMeshes(childNode, outMeshes);
			}
		}
	}
}

bool ModelLoader::LoadFBX(std::string aPath, int aMode)
{
	auto slash = aPath.find_last_of('/');
	std::string modelName = aPath.substr(slash + 1, aPath.size());
	modelName = modelName.substr(0, modelName.find_last_of('.'));
	std::string folder = aPath.substr(0, slash) + "/";
	std::string filePath = folder + modelName + ".erc";

	switch (aMode)
	{
		//case 0:
		//	return (remove(filePath.c_str()) == 0);
		//	break;
		//case 1:
		//	remove(filePath.c_str());
		//	break;
	case 2:
		if (std::filesystem::exists(filePath))
		{
			return false;
		}
		break;
	default:
		break;
	}

	// OLD
	Assimp::Importer importer;
	//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	unsigned flags =
		aiProcess_CalcTangentSpace
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
		return false;
	}

	if (aiScene->mNumAnimations > 0)
	{
		return LoadAnimation((void*)aiScene, aPath);
	}

	bool hasBones = false;
	for (int i = 0; i < aiScene->mNumMeshes; i++)
	{
		if (aiScene->mMeshes[i]->HasBones())
		{
			hasBones = true;
			break;
		}
	}

	if (hasBones)
	{
		unsigned flags2 =
			aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_SortByPType
			| aiProcess_MakeLeftHanded
			| aiProcess_FlipWindingOrder
			| aiProcess_FlipUVs
			// Caution, might lose data
			| aiProcess_OptimizeGraph
			| aiProcess_OptimizeMeshes

			| aiProcessPreset_TargetRealtime_MaxQuality;
		flags2 &= ~aiProcess_JoinIdenticalVertices;

		aiScene = importer.ReadFile(aPath.data(), flags2);
	}

	std::vector<std::shared_ptr<Mesh>> meshes;
	Skeleton skeleton;

	std::vector<std::string> materials;
	for (unsigned materialIndex = 0; materialIndex < aiScene->mNumMaterials; materialIndex++)
	{
		auto& material = aiScene->mMaterials[materialIndex];
		materials.push_back(material->GetName().C_Str());
	}

	HRESULT result;

	float radius = 0;
	meshes.reserve(aiScene->mNumMeshes);
	for (unsigned meshIndex = 0; meshIndex < aiScene->mNumMeshes; meshIndex++)
	{
		std::vector<std::unique_ptr<BoneData>> boneData;
		auto& aiMesh = aiScene->mMeshes[meshIndex];
		auto mat = aiScene->mMeshes[meshIndex]->mMaterialIndex;

		if (aiMesh->HasBones())
		{
			hasBones = true;
			for (int i = 0; i < aiMesh->mNumVertices; i++)
			{
				boneData.push_back(std::make_unique<BoneData>());
			}

			unsigned int boneIndex = 0;
			for (unsigned int i = 0; i < aiMesh->mNumBones; i++)
			{
				std::string BoneName(aiMesh->mBones[i]->mName.data);
				if (skeleton.myJointNameToIndex.find(BoneName) == skeleton.myJointNameToIndex.end())
				{
					boneIndex = skeleton.myJoints.size();
					ModelLoader::Skeleton::Joint joint;

					float transform[16];
					auto matrix = aiMesh->mBones[i]->mOffsetMatrix;
					matrix = matrix.Transpose();

					memcpy(&transform[0], &(matrix.a1), sizeof(float) * 16);
					memcpy(&(joint.myBindPoseInverse[0]), &transform[0], sizeof(float) * 16);
					_memccpy(&(joint.myName), BoneName.c_str(), '\0', 32);
					memcpy(&(joint.myIndex), &boneIndex, sizeof(int));

					skeleton.myJointNameToIndex[BoneName] = boneIndex;
					skeleton.myJoints.push_back(joint);
				}
				else
				{
					boneIndex = skeleton.myJointNameToIndex[BoneName];
				}

				for (unsigned int j = 0; j < aiMesh->mBones[i]->mNumWeights; j++)
				{
					unsigned int vertexID = aiMesh->mBones[i]->mWeights[j].mVertexId;
					float weight = aiMesh->mBones[i]->mWeights[j].mWeight;
					boneData[vertexID]->AddBoneData(boneIndex, weight);
				}
			}
		}

		std::vector<Vertex> vertices;
		vertices.reserve(aiMesh->mNumVertices);
		for (unsigned vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; ++vertexIndex)
		{
			Vertex vertex = { 0 };

			vertex.x = aiMesh->mVertices[vertexIndex].x;
			vertex.y = aiMesh->mVertices[vertexIndex].y;
			vertex.z = aiMesh->mVertices[vertexIndex].z;
			vertex.w = 1.0f;

			float length = vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z;

			radius = radius < length ? length : radius;

			vertex.r = 1;
			vertex.g = 1;
			vertex.b = 1;
			vertex.a = 1;

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
			if (aiMesh->HasBones())
			{
				for (int boneID = 0; boneID < 4; boneID++)
				{
					vertex.ids[boneID] = boneData[vertexIndex]->ids[boneID];
				}
				for (int weight = 0; weight < 4; weight++)
				{
					vertex.weights[weight] = boneData[vertexIndex]->weights[weight];
				}
			}

			vertices.push_back(vertex);
		}

		std::vector<UINT> indices;
		indices.reserve(static_cast<size_t>(aiMesh->mNumFaces) * static_cast<size_t>(3u));
		for (unsigned faceIndex = 0; faceIndex < aiMesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = aiMesh->mFaces[faceIndex];
			for (unsigned indexIndex = 0; indexIndex < face.mNumIndices; ++indexIndex)
			{
				indices.emplace_back(face.mIndices[indexIndex]);
			}
		}
		indices.shrink_to_fit();

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->myVertices = vertices;
		mesh->myIndices = indices;
		mesh->myTexture = materials[mat];
		meshes.push_back(mesh);
	}

	// importer
	//std::ifstream rf;
	//rf.open(filePath, std::ios::in | std::ios::binary);
	//std::string ercData = { std::istreambuf_iterator<char>(rf), std::istreambuf_iterator<char>() };
	//rf.close();
	//
	//int index = 0;
	//int mMeshes = *reinterpret_cast<int*>(&ercData[0]);
	//index += sizeof(int);
	//
	//std::vector<Mesh> mMeshV;
	//for (size_t i = 0; i < mMeshes; i++)
	//{
	//	Mesh mesh;
	//
	//	int mNumVerts = *reinterpret_cast<int*>(&ercData[index]);
	//	index += sizeof(int);
	//	mesh.myVertices.resize(mNumVerts);
	//	int size = sizeof(Vertex) * mNumVerts;
	//	memcpy(&mesh.myVertices[0], &ercData[index], size);
	//	index += size;
	//	int mNumInds = *reinterpret_cast<int*>(&ercData[index]);
	//	index += sizeof(int);
	//	mesh.myIndices.resize(mNumInds);
	//	size = sizeof(int) * mNumInds;
	//	memcpy(&mesh.myIndices[0], &ercData[index], size);
	//	index += size;
	//	char mTexPath[128];
	//	size = sizeof(char) * 128;
	//	memcpy(&mTexPath[0], &ercData[index], size);
	//	index += size;
	//
	//	mesh.myTexture = std::string(mTexPath);
	//	mesh.myTexture = std::string(mesh.myTexture.begin(), mesh.myTexture.end() - 1);
	//
	//	mMeshV.push_back(mesh);
	//}
	//
	//for (auto& mesh : mMeshV)
	//{
	//	printf("Vertices: %i\n", (int)mesh.myVertices.size());
	//	printf("Indices: %i\n", (int)mesh.myIndices.size());
	//	printf("Texture Path: %s\n---------------------------------------\n", mesh.myTexture.c_str());
	//}

	radius = std::sqrtf(radius);

	UINT size = meshes.size();
	std::ofstream wf(filePath.c_str(), std::ios::out | std::ios::binary);
	wf.write((char*)&size, sizeof(UINT));
	wf.write((char*)&radius, sizeof(float));
	wf.write((char*)&hasBones, sizeof(bool));

	if (hasBones)
	{
		int numJoints = skeleton.myJoints.size();
		wf.write((char*)&numJoints, sizeof(int));
		wf.write((char*)&skeleton.myJoints[0], sizeof(Skeleton::Joint) * numJoints);
		int numNames = skeleton.myJointNameToIndex.size();
		wf.write((char*)&numNames, sizeof(int));
		for (auto& jointName : skeleton.myJointNameToIndex)
		{
			wf.write((char*)&jointName.first[0], sizeof(char) * 32);
			wf.write((char*)&jointName.second, sizeof(int));
		}
	}

	for (const auto& mesh : meshes)
	{
		int numVerts = mesh->myVertices.size();
		wf.write((char*)&numVerts, sizeof(int));
		wf.write((char*)&(mesh->myVertices[0]), sizeof(Vertex) * numVerts);
		int numInds = mesh->myIndices.size();
		wf.write((char*)&numInds, sizeof(int));
		wf.write((char*)&(mesh->myIndices[0]), sizeof(UINT) * numInds);
		char buffer[128];
		memset(buffer, 0, 128);
		int index = 0;
		for (int i = 0; i < mesh->myTexture.size(); i++)
		{
			buffer[i] = mesh->myTexture[i];
			index++;
		}
		for (int i = index + 1; i < (128 - index); i++)
		{
			buffer[i] = 0;
		}
		wf.write(&buffer[0], sizeof(char) * 128);
	}
	wf.close();

	return true;
}

