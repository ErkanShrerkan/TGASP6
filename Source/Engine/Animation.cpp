#include "pch.h"
#include "Animation.h"
#include <CommonUtilities\Math\Quaternion.h>

namespace SE
{
	void CAnimation::AddJoint(std::string aJointName)
	{
		myAnimation[aJointName];
	}

	CAnimation::STrack& CAnimation::GetTrack(std::string aJointName)
	{
		return myAnimation[aJointName];
	}

	void CAnimation::CalculateFrames()
	{
		for (auto& node : myAnimation)
		{
			Quaternion rot;
			float4 pos(0, 0, 0, 1);
			float3 scl(1, 1, 1);

			Matrix4x4f nodeTransform = myNodeTransforms[node.first];
			CAnimation::STrack& track = node.second;

			for (int frame = 0; frame < myDurationInFrames; frame++)
			{
				for (int i = 0; i < track.myTranslationKeys.size(); i++)
				{
					if (track.myTranslationKeys.size() == 1)
					{
						memcpy(&pos, &track.myTranslationKeys[0].pos, sizeof(float) * 3);
						break;
					}

					if (track.myTranslationKeys[i].time >= frame && i != 0)
					{
						float3 lastPos = track.myTranslationKeys[i - 1].pos.xyz;
						float3 nextPos = track.myTranslationKeys[i].pos.xyz;
						float lastFrame = track.myTranslationKeys[i - 1].time;
						float nextFrame = track.myTranslationKeys[i].time;
						float lerpValue = (frame - lastFrame) / (nextFrame - lastFrame);
						pos.xyz = Math::Lerp(lastPos, nextPos, lerpValue);
						break;
					}
				}

				for (int i = 0; i < track.myRotationKeys.size(); i++)
				{
					if (track.myRotationKeys.size() == 1)
					{
						rot.x = track.myRotationKeys[0].rotation.x;
						rot.y = track.myRotationKeys[0].rotation.y;
						rot.z = track.myRotationKeys[0].rotation.z;
						rot.w = track.myRotationKeys[0].rotation.w;
						rot = rot.normalize();
						break;
					}

					if (track.myRotationKeys[i].time >= frame && i != 0)
					{
						Quaternion lastRot;
						lastRot.x = track.myRotationKeys[i - 1].rotation.x;
						lastRot.y = track.myRotationKeys[i - 1].rotation.y;
						lastRot.z = track.myRotationKeys[i - 1].rotation.z;
						lastRot.w = track.myRotationKeys[i - 1].rotation.w;
						lastRot = lastRot.normalize();
						Quaternion nextRot;
						nextRot.x = track.myRotationKeys[i].rotation.x;
						nextRot.y = track.myRotationKeys[i].rotation.y;
						nextRot.z = track.myRotationKeys[i].rotation.z;
						nextRot.w = track.myRotationKeys[i].rotation.w;
						nextRot = nextRot.normalize();

						float lastFrame = track.myRotationKeys[i - 1].time;
						float nextFrame = track.myRotationKeys[i].time;
						float lerpValue = (frame - lastFrame) / (nextFrame - lastFrame);
						rot = rot.slerp(lastRot, nextRot, lerpValue);
						rot = rot.normalize();
						break;
					}
				}

				for (int i = 0; i < track.myScaleKeys.size(); i++)
				{
					if (track.myScaleKeys.size() == 1)
					{
						memcpy(&scl, &track.myScaleKeys[0].scale, sizeof(float) * 3);
						break;
					}

					if (track.myScaleKeys[i].time >= frame && i != 0)
					{
						float3 lastScl = track.myScaleKeys[i - 1].scale;
						float3 nextScl = track.myScaleKeys[i].scale;
						float lastFrame = track.myScaleKeys[i - 1].time;
						float nextFrame = track.myScaleKeys[i].time;
						float lerpValue = (frame - lastFrame) / (nextFrame - lastFrame);
						scl = Math::Lerp(lastScl, nextScl, lerpValue);
						break;
					}
				}

				Matrix4x4f rotation;
				Matrix4x4f translation;
				Matrix4x4f scaling;

				scaling(1, 1) = scl.x;
				scaling(2, 2) = scl.y;
				scaling(3, 3) = scl.z;

				translation.SetRow(4, pos);
				rot.getRotationMatrix(rotation);

				Matrix4x4f transform = scaling * rotation * translation;
				myNodeFrameTransforms[node.first].push_back(transform);
			}
		}
	}
}
