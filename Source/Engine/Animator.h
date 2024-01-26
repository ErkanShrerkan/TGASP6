#pragma once
#include <string>
#include <unordered_map>
#include <CommonUtilities\Math\Quaternion.h>
#include "Model.h"

namespace SE
{
	class CAnimation;
	class CAnimator
	{
	public:
		CAnimator(CModel* aModel);
		void AddAnimation(std::string aPath, std::string anAlias);
		void SetFallbackAnimation(std::string anAnim);
		void Update(float aDeltaTime);
		void Play(std::string anAnim, bool aLoop = true, std::string aFallbackAnim = "", bool aLoopBlend = true);
		std::unordered_map<std::string, Matrix4x4f>& GetNodeTransforms() { return myNodeTransforms; }
		std::vector<Matrix4x4f>& GetJointTransforms() { return myJointTransforms; }
		Matrix4x4f GetJointTransform(std::string aJoint);
		FORCEINLINE bool IsOver() const { return myIsOver; }
	private:
		Quaternion ConvertAssimpQuaternion(float4 aQuaternion);
		void ApplyPoseToJoints(const std::string& aNodeName, Matrix4x4f& aParentTransform);
		bool IsValidAnim(CAnimation* anAnim);
		std::string GetRootJoint();
		void UpdateTime(float aDeltaTime);
		void UpdateBlend(float aDeltaTime);
		//void UpdatePose(float aDeltaTime);
		void UpdateNodeTransform(float4x4& aNodeTransform, CAnimation& anAnimation, const std::string& aNodeName, float aTime);
		void ApplyBlend(const std::string& aNodeName, float4x4& aNodeTransform);
		void ApplySingle(const std::string& aNodeName, float4x4& aNodeTransform);
		void ApplyFrameData(const std::string& aNodeName, float4x4& aNodeTransform);
		bool ShouldBlend(const std::string& aNodeName) const;

		std::unordered_map<std::string, CAnimation*> myAnimations;
		std::unordered_map<std::string, Matrix4x4f> myNodeTransforms;
		std::vector<Matrix4x4f> myJointTransforms;
		std::string myCurrentAnim;
		std::string myBlendAnim;
		std::string myFallBackAnim;
		CModel* myModel = nullptr;
		CAnimation* myCurrentAnimPtr = nullptr;
		bool myHasFallback = false;
		bool myLooped = false;
		bool myShouldLoop = false;
		bool myIsOver = false;
		bool myBlendIsOver = false;
		bool myLoopBlend = true;
		float myBlend;
		float myTime;
		float myBlendTime;
		unsigned myCurrentFrame;
		unsigned myBlendFrame;
	};
}

