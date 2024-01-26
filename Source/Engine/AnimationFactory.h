#pragma once
#include <string>
#include <unordered_map>

namespace SE
{
	class CAnimation;
	class CAnimator;
	class CAnimationFactory
	{
	public:
		CAnimation* GetAnimation(std::string aPath);
		static CAnimationFactory& GetInstance()
		{
			static CAnimationFactory instance;
			return instance;
		}

		CAnimationFactory(const CAnimationFactory& aFactory) = delete;
		void operator=(const CAnimationFactory& aFactory) = delete;

	private:
		CAnimationFactory() {}
		CAnimation* CreateAnimation(std::string aPath);
		CAnimation* LoadAnimation(std::string aPath);

		std::unordered_map<std::string, CAnimation*> myAnimations;
	};
}

