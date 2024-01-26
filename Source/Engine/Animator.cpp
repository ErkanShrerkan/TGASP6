#include "pch.h"
#include "Animator.h"
#include "AnimationFactory.h"
#include "Animation.h"
#include "Model.h"

namespace SE
{
	CAnimator::CAnimator(CModel* aModel)
		: myModel(aModel)
	{
		myTime = 0;
		if (aModel)
		{
			myJointTransforms.resize(myModel->GetSkeleton().myJoints.size());
		}
	}

	void CAnimator::AddAnimation(std::string aPath, std::string anAlias)
	{
		CAnimation* newAnim = CAnimationFactory::GetInstance().GetAnimation(aPath);
		//if (IsValidAnim(newAnim))
		//{
		//myNodeTransforms.clear();
		myNodeTransforms = newAnim->GetNodeTransforms();
		myAnimations[anAlias] = newAnim;
		//myCurrentAnim = anAlias;
		//}
	}

	void CAnimator::SetFallbackAnimation(std::string anAnim)
	{
		myHasFallback = true;
		myFallBackAnim = anAnim;
		if (anAnim.empty())
		{
			myHasFallback = false;
		}
	}

	void CAnimator::Update(float aDeltaTime)
	{
		if (!myModel)
		{
			return;
		}

		myCurrentAnimPtr = myAnimations[myCurrentAnim];
		if (!myCurrentAnimPtr)
		{
			return;
		}

		UpdateTime(aDeltaTime);
		UpdateBlend(aDeltaTime);

		Matrix4x4f worldSpaceTransform;
		ApplyPoseToJoints(myCurrentAnimPtr->myRootName, worldSpaceTransform);
	}

	void CAnimator::Play(std::string anAnim, bool aLoop, std::string aFallbackAnim, bool aLoopBlend)
	{
		assert(myAnimations.find(anAnim) != myAnimations.end());
		if (myCurrentAnim != anAnim)
		{
			myBlendAnim = myCurrentAnim;
			myBlendTime = myTime;
		}
		if (myBlendAnim.empty())
		{
			myBlendAnim = anAnim;
		}

		myBlend = 0;
		myCurrentAnim = anAnim;
		myShouldLoop = aLoop;
		myTime = 0;
		myLoopBlend = aLoopBlend;

		myHasFallback = !aFallbackAnim.empty();
		myFallBackAnim = aFallbackAnim;
	}

	Matrix4x4f CAnimator::GetJointTransform(std::string aJoint)
	{
		int i = myModel->GetSkeleton().myJointNameToIndex[aJoint];
		return Matrix4x4f::GetFastInverse(myModel->GetSkeleton().myJoints[i].myBindPoseInverse) * myJointTransforms[i];
	}

	Quaternion CAnimator::ConvertAssimpQuaternion(float4 aQuaternion)
	{
		Quaternion q;
		q.x = aQuaternion.x;
		q.y = aQuaternion.y;
		q.z = aQuaternion.z;
		q.w = aQuaternion.w;
		return q;
	}

	void CAnimator::ApplyPoseToJoints(const std::string& aNodeName, Matrix4x4f& aParentTransform)
	{
		CModel::SSkeleton& skeleton = myModel->GetSkeleton();

		Matrix4x4f nodeTransform = myNodeTransforms[aNodeName];

		ApplyFrameData(aNodeName, nodeTransform);

		Matrix4x4f currentTransform = nodeTransform * aParentTransform;

		auto joint = skeleton.myJointNameToIndex.find(aNodeName);
		if (joint != skeleton.myJointNameToIndex.end())
		{
			int jointIndex = joint->second;
			myJointTransforms[jointIndex] = skeleton.myJoints[jointIndex].myBindPoseInverse * currentTransform * myCurrentAnimPtr->myRootTransform;
		}

		const auto& children = myCurrentAnimPtr->GetJointHierarchy()[aNodeName];
		for (const auto& childName : children)
		{
			ApplyPoseToJoints(childName, currentTransform);
		}
	}

	bool CAnimator::IsValidAnim(CAnimation* anAnim)
	{
		if (!anAnim)
		{
			return false;
		}

		auto& skeleton = myModel->GetSkeleton();
		for (auto& joint : anAnim->myJointHierarchy)
		{
			if (joint.first != "RootNode")
			{
				for (auto& name : joint.second)
				{
					if (skeleton.myJointNameToIndex.find(name) == skeleton.myJointNameToIndex.end())
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	std::string CAnimator::GetRootJoint()
	{
		bool foundRoot = false;
		std::string rootContestant;

		int i = 0;
		for (auto& name : myCurrentAnimPtr->GetJointHierarchy())
		{
			i++;
			rootContestant = name.first;
			if (i)
			{
				break;
			}
		}

		while (!foundRoot)
		{
			bool nameFound = false;
			for (auto& jointNames : myCurrentAnimPtr->GetJointHierarchy())
			{
				for (auto& jointName : jointNames.second)
				{
					nameFound = (jointName == rootContestant);

					if (nameFound)
					{
						rootContestant = jointName;
						break;
					}
				}
			}

			if (!nameFound)
			{
				foundRoot = true;
			}
		}

		return rootContestant;
	}

	void CAnimator::UpdateTime(float aDeltaTime)
	{
		myTime += aDeltaTime;

		myIsOver = (int)myIsOver * (int)!myIsOver;
		if (!(myTime >= myCurrentAnimPtr->myDurationInSeconds))
		{
			return;
		}

		myIsOver = true;
		if (myHasFallback && !myShouldLoop)
		{
			myBlendAnim = myCurrentAnim;
			myBlendTime = myTime;
			myCurrentAnim = myFallBackAnim;
			myBlend = 0;
			myHasFallback = false;
			myTime -= myCurrentAnimPtr->myDurationInSeconds;
			myShouldLoop = true;
			return;
		}

		if (!myHasFallback && !myShouldLoop)
		{
			myTime = myCurrentAnimPtr->myDurationInSeconds;
			return;
		}

		myTime -= myCurrentAnimPtr->myDurationInSeconds;
	}

	void CAnimator::UpdateBlend(float aDeltaTime)
	{
		float blendMult = 7.5f;
		myBlend = myBlend + aDeltaTime * blendMult >= 1 ? 1 : myBlend + aDeltaTime * blendMult;
		myBlendTime += aDeltaTime;

		if (!(myBlend < 1 && myCurrentAnim != myBlendAnim))
		{
			return;
		}

		CAnimation* blendAnim = myAnimations[myBlendAnim];
		//float blendTimeInFrames = myBlendTime * blendAnim->myFPS;
		myBlendIsOver = (int)myBlendIsOver * (int)!myBlendIsOver;
		if (!(myBlendTime >= blendAnim->myDurationInSeconds))
		{
			return;
		}

		myBlendIsOver = true;
		if (myLoopBlend)
		{
			myBlendTime -= blendAnim->myDurationInSeconds;
			return;
		}

		myBlendTime = blendAnim->myDurationInSeconds;
	}

	void CAnimator::UpdateNodeTransform(float4x4& aNodeTransform, CAnimation& anAnimation, const std::string& aNodeName, float aTime)
	{
		const auto& frameTransforms = anAnimation.GetNodeFrameTransforms()[aNodeName];
		aNodeTransform = frameTransforms[0];

		if (frameTransforms.size() <= 1)
		{
			return;
		}

		const float frame = aTime * anAnimation.myFPS;
		const float frameFloor = std::floor(frame);
		const float frameCeil = std::ceil(frame);
		const float lastFrame = frameFloor >= anAnimation.myDurationInFrames ? anAnimation.myDurationInFrames - 1 : frameFloor;
		const float nextFrame = frameCeil >= anAnimation.myDurationInFrames ? (anAnimation.myDurationInFrames - 1) * !myShouldLoop : frameCeil;
		const float lerpValue = (frame - lastFrame) / (frameCeil - lastFrame);

		const auto& lastTransform = frameTransforms[(int)lastFrame];
		const auto& nextTransform = frameTransforms[(int)nextFrame];
		const float3 position = Math::Lerp(lastTransform.GetPosition(), nextTransform.GetPosition(), lerpValue);

		const Quaternion q0(lastTransform);
		const Quaternion q1(nextTransform);
		Quaternion rot;

		rot = rot.slerp(q0, q1, lerpValue);
		rot = rot.normalize();
		rot.getRotationMatrix(aNodeTransform);

		aNodeTransform.SetRow(4, { position, 1 });
	}

	void CAnimator::ApplyBlend(const std::string& aNodeName, float4x4& aNodeTransform)
	{
		CAnimation& blendAnim = *myAnimations[myBlendAnim];

		Matrix4x4f transform0;
		UpdateNodeTransform(transform0, blendAnim, aNodeName, myBlendTime);

		Matrix4x4f transform1;
		UpdateNodeTransform(transform1, *myCurrentAnimPtr, aNodeName, myTime);

		const float3 position = Math::Lerp(transform0.GetPosition(), transform1.GetPosition(), myBlend);
		const Quaternion q0(transform0);
		const Quaternion q1(transform1);
		Quaternion rot;

		rot = rot.slerp(q0, q1, myBlend);
		rot = rot.normalize();
		rot.getRotationMatrix(aNodeTransform);

		aNodeTransform.SetRow(4, { position, 1 });
	}

	void CAnimator::ApplySingle(const std::string& aNodeName, float4x4& aNodeTransform)
	{
		UpdateNodeTransform(aNodeTransform, *myCurrentAnimPtr, aNodeName, myTime);
	}

	void CAnimator::ApplyFrameData(const std::string& aNodeName, float4x4& aNodeTransform)
	{
		auto& tracks = myCurrentAnimPtr->GetTracks();
		if (tracks.find(aNodeName) == tracks.end())
		{
			return;
		}

		if (ShouldBlend(aNodeName))
		{
			ApplyBlend(aNodeName, aNodeTransform);
			return;
		}

		ApplySingle(aNodeName, aNodeTransform);
	}

	bool CAnimator::ShouldBlend(const std::string& aNodeName) const
	{
		CAnimation& blendAnim = *myAnimations.at(myBlendAnim);
		return myBlend < 1.f && myBlendAnim != myCurrentAnim && blendAnim.GetTracks().find(aNodeName) != blendAnim.GetTracks().end();
	}
}
