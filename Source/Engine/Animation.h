#pragma once
#include <vector>
#include <unordered_map>

namespace SE
{
	class CAnimation
	{
		friend class CAnimationFactory;
		friend class CAnimator;

		struct STranslationKey
		{
			float4 pos;
			float time;
		};

		struct SScaleKey
		{
			float3 scale;
			float time;
		};

		struct SRotationKey
		{
			float4 rotation;
			float time;
		};

		struct STrack
		{
			std::vector<STranslationKey> myTranslationKeys;
			std::vector<SScaleKey> myScaleKeys;
			std::vector<SRotationKey> myRotationKeys;
		};

	public:
		void AddJoint(std::string aJointName);
		STrack& GetTrack(std::string aJointName);
		std::unordered_map<std::string, Matrix4x4f>& GetNodeTransforms() { return myNodeTransforms; }
		std::unordered_map<std::string, std::vector<Matrix4x4f>>& GetNodeFrameTransforms() { return myNodeFrameTransforms; }
		std::unordered_map<std::string, std::vector<std::string>>& GetJointHierarchy() { return myJointHierarchy; }
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
		std::unordered_map<std::string, STrack>& GetTracks()
		{
			return myAnimation;
		}

	private:
		void CalculateFrames();

		std::unordered_map<std::string, std::vector<std::string>> myJointHierarchy;
		std::unordered_map<std::string, std::vector<Matrix4x4f>> myNodeFrameTransforms;
		std::unordered_map<std::string, Matrix4x4f> myNodeTransforms;
		std::unordered_map<std::string, STrack> myAnimation;
		std::string myRootName;
		Matrix4x4f myRootTransform;
		float myDurationInFrames;
		float myDurationInSeconds;
		float myFPS;
	};
}

