#include "pch.h"
#include "AnimationFactory.h"
#include <fstream>
#include <sstream>
#include "Animation.h"

namespace SE
{
	CAnimation* CAnimationFactory::GetAnimation(std::string aPath)
	{
		return myAnimations.find(aPath) == myAnimations.end() ? CreateAnimation(aPath) : LoadAnimation(aPath);
	}

	CAnimation* CAnimationFactory::CreateAnimation(std::string aPath)
	{
		std::ifstream t(aPath, std::ios::in | std::ios::binary);
		std::stringstream buffer;
		buffer << t.rdbuf();
		t.close();
		std::string myrData = buffer.str();
		if (myrData.empty())
		{
			return nullptr;
		}

		CAnimation* anim = new CAnimation();

		int index = 0;

		anim->myRootTransform = *reinterpret_cast<Matrix4x4f*>(&myrData[0]);
		index += sizeof(float) * 16;

		unsigned size = *reinterpret_cast<unsigned*>(&myrData[index]);
		index += sizeof(unsigned);

		char rootName[32];
		_memccpy(&rootName[0], &myrData[index], '\0', 32);
		index += 32;

		anim->myRootName = rootName;

		auto& hierarchy = anim->GetJointHierarchy();
		for (unsigned i = 0; i < size; i++)
		{
			char parentName[32];
			_memccpy(&parentName[0], &myrData[index], '\0', 32);
			index += 32;

			unsigned numChildren = *reinterpret_cast<unsigned*>(&myrData[index]);
			index += sizeof(unsigned);

			for (unsigned j = 0; j < numChildren; j++)
			{
				char childName[32];
				_memccpy(&childName[0], &myrData[index], '\0', 32);
				index += 32;

				hierarchy[parentName].push_back(childName);
			}
		}

		size = *reinterpret_cast<unsigned*>(&myrData[index]);
		index += sizeof(unsigned);

		auto& jointTransforms = anim->GetNodeTransforms();
		for (unsigned i = 0; i < size; i++)
		{
			char nodeName[32];
			_memccpy(&nodeName[0], &myrData[index], '\0', 32);
			index += 32;

			Matrix4x4f transform;
			memcpy(&transform[0], &myrData[index], sizeof(float) * 16);
			index += sizeof(float) * 16;

			jointTransforms[nodeName] = transform;
		}

		anim->myDurationInFrames = *reinterpret_cast<float*>(&myrData[index]);
		index += sizeof(float);
		anim->myDurationInSeconds = *reinterpret_cast<float*>(&myrData[index]);
		index += sizeof(float);
		anim->myFPS = *reinterpret_cast<float*>(&myrData[index]);
		index += sizeof(float);

		size = *reinterpret_cast<unsigned*>(&myrData[index]);
		index += sizeof(unsigned);

		for (unsigned i = 0; i < size; i++)
		{
			CAnimation::STrack track;

			char name[32];
			_memccpy(&name[0], &myrData[index], '\0', 32);
			index += 32;

			int inc;
			unsigned tkSize = *reinterpret_cast<unsigned*>(&myrData[index]);
			index += sizeof(unsigned);
			track.myTranslationKeys.resize(tkSize);
			inc = sizeof(CAnimation::STranslationKey) * tkSize;
			memcpy(&track.myTranslationKeys[0], &myrData[index], inc);
			index += inc;

			unsigned rkSize = *reinterpret_cast<unsigned*>(&myrData[index]);
			index += sizeof(unsigned);
			track.myRotationKeys.resize(rkSize);
			inc = sizeof(CAnimation::SRotationKey) * rkSize;
			memcpy(&track.myRotationKeys[0], &myrData[index], inc);
			index += inc;

			unsigned skSize = *reinterpret_cast<unsigned*>(&myrData[index]);
			index += sizeof(unsigned);
			track.myScaleKeys.resize(skSize);
			inc = sizeof(CAnimation::SScaleKey) * skSize;
			memcpy(&track.myScaleKeys[0], &myrData[index], inc);
			index += inc;

			anim->myAnimation[&name[0]] = track;
		}

		anim->CalculateFrames();

		return anim;
	}

	CAnimation* CAnimationFactory::LoadAnimation(std::string aPath)
	{
		return myAnimations[aPath];
	}
}
