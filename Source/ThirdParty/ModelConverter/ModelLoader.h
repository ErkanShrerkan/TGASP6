#pragma once
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>

class ModelLoader
{
	struct Matrix
	{
		float matrix[16];
	};

	struct BoneData
	{
		std::vector<unsigned> ids;
		std::vector<float> weights;
		int index = 0;

		void AddBoneData(unsigned id, float weight)
		{
			if (index < 4)
			{
				ids[index] = id;
				weights[index] = weight;
				index++;
			}
		}

		BoneData()
		{
			for (int i = 0; i < 4; i++)
			{
				ids.push_back(0);
				weights.push_back(0);
			}
		}
		~BoneData()
		{
			ids.clear();
			weights.clear();
		}
	};

	struct Vertex
	{
		float x, y, z, w;
		float r, g, b, a;
		float u, v;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;

		unsigned ids[4];
		float weights[4];

		friend bool operator==(Vertex& v0, Vertex& v1)
		{
			return !(memcmp(&v0, &v1, sizeof(Vertex)));
		}
	};

	struct Mesh
	{
		std::vector<Vertex> myVertices;
		std::vector<unsigned int> myIndices;
		std::string myTexture;
	};

	struct Skeleton
	{
		struct Joint
		{
			unsigned myIndex;
			char myName[32];
			float myBindPoseInverse[16];
			//std::vector<Matrix> myAnim;
		};
		std::vector<Joint> myJoints;
		std::unordered_map<std::string, int> myJointNameToIndex;
	};

	class Animation
	{
	public:
		struct TranslationKey
		{
			float x, y, z, w;
			float time;
		};

		struct RotationKey
		{
			float x, y, z, w;
			float time;
		};

		struct ScaleKey
		{
			float x, y, z;
			float time;
		};

		struct Track
		{
			std::vector<TranslationKey> myTranslationKeys;
			std::vector<ScaleKey> myScaleKeys;
			std::vector<RotationKey> myRotationKeys;
		};

		std::unordered_map<std::string, Track> myAnimation;
		float myDurationInFrames;
		float myDurationInSeconds;
		float myFPS;

	public:
		void AddJoint(std::string aJointName) { myAnimation[aJointName]; }
		Track& GetTrack(std::string aJointName) { return myAnimation[aJointName]; }
		bool JointExists(std::string aJointName)
		{
			return myAnimation.find(aJointName) != myAnimation.end();
		}
		void SetDurationAndFPS(float frames, float fps)
		{
			myDurationInFrames = frames;
			myFPS = fps;
			myDurationInSeconds = frames / fps;
		}
		std::unordered_map<std::string, Track>& GetTracks()
		{
			return myAnimation;
		}
	};

	void* FindAnimNode(void* anAnimation, std::string aName);
	void ReadNodeHierarchy(
		void* aScene, void* aNode, 
		std::unordered_map<std::string, std::vector<std::string>>& aNodeHierarchy, 
		std::string aParentName, bool aParentWasNull);
	bool GatherSkeletonData(FbxNode* aNode, Skeleton& aSkeleton, unsigned anIndex, unsigned aParentIndex);
	bool GatherSkeletonData(FbxNode* aNode, std::vector<FbxNode*>& outBones, unsigned anIndex, unsigned aParentIndex);
	std::string GetRootJoint(std::unordered_map<std::string, std::vector<std::string>>& aJointHierarchy);
	bool LoadFBXWithBones(std::string aPath, std::unordered_map<std::string, unsigned>& parentNodes);
	void GatherFBXMeshes(FbxNode* aNode, std::vector<FbxNode*>& outMeshes);
	bool LoadAnimation(void* aScene, std::string aPath);

public:
	bool DeleteFileML(std::string aPath);
	bool LoadFBX(std::string aFilePath, int aMode);

	std::unordered_map<std::string, Matrix> myNodeTransforms;
};

