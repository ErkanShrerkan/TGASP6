#pragma once
#include <string>
namespace CommonUtilities
{
	namespace Internal
	{
		typedef unsigned char Byte;
		inline uint32_t Hash(const Byte* someBytes, size_t anAmountOfBytes)
		{
			uint32_t hash = 21;
			uint32_t prime = 325996157;
			for (size_t i = 0; i < anAmountOfBytes; i++)
			{
				hash ^= someBytes[i];
				hash *= prime;
			}
			return hash;
		}
	}

	template <class Type>
	inline uint32_t Hash(const Type& aKey)
	{
		return Internal::Hash(reinterpret_cast<const Internal::Byte*>(&aKey), sizeof(aKey));
	}
	template <>
	inline uint32_t Hash(const std::string& aKey)
	{
		return Internal::Hash(reinterpret_cast<const Internal::Byte*>(aKey.c_str()), aKey.size());
	}
}
