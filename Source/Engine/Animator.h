#pragma once
#include <string>
#include <unordered_map>
#include <CommonUtilities\Math\Quaternion.h>
#include "Model.h"
#include <functional>

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
		void Play(const std::string& anAnim, bool aLoop = true, const std::string& aFallbackAnim = "", bool aLoopBlend = true, float aBlendTransitionTimeIn = 1.0f / 7.5f, float aBlendTransitionTimeOut = 1.0f / 7.5f);
		void SetOnOverCallback(std::function<void()> aCallback);
		std::unordered_map<std::string, Matrix4x4f>& GetNodeTransforms() { return myNodeTransforms; }
		std::vector<Matrix4x4f>& GetJointTransforms() { return myJointTransforms; }
		Matrix4x4f GetJointTransform(std::string aJoint) const;
		FORCEINLINE bool IsOver() const { return myIsOver; }
	private:
		Quaternion ConvertAssimpQuaternion(float4 aQuaternion);
		void ApplyPoseToJoints(const std::string& aNodeName, Matrix4x4f& aParentTransform);
		bool IsValidAnim(CAnimation* anAnim);
		std::string GetRootJoint();
		void UpdateTime(float aDeltaTime);
		void UpdateBlend(float aDeltaTime);
		void UpdateNodeTransform(float4x4& aNodeTransform, CAnimation& anAnimation, const std::string& aNodeName, float aTime);
		void ApplyBlend(const std::string& aNodeName, float4x4& aNodeTransform);
		void ApplySingle(const std::string& aNodeName, float4x4& aNodeTransform);
		void CalculateJointTransform(const std::string& aNodeName, float4x4& aNodeTransform);
		void ApplyJointTransform(const std::string& aNodeName, const float4x4& aCurrentTransform);
		bool ShouldBlend(const std::string& aNodeName) const;

		std::unordered_map<std::string, CAnimation*> myAnimations;
		std::unordered_map<std::string, Matrix4x4f> myNodeTransforms;
		std::vector<Matrix4x4f> myJointTransforms;
		std::string myCurrentAnim;
		std::string myBlendAnim;
		std::string myFallBackAnim;
		std::function<void()> myOnOverCallback;
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
		float myBlendTransitionTimeInSeconds;
		float myFallbackBlendTransitionTimeInSeconds;
		unsigned myCurrentFrame;
		unsigned myBlendFrame;
	};
}

