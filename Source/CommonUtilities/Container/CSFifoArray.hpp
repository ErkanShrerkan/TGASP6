#pragma once
#include <cstring>
namespace CommonUtilities
{
	namespace CStyle
	{
		template <class T, size_t size>
		class FIFOArray
		{
		public:
			void Push(const T& aValue)
			{
				// Pop c style queue
				memmove(myData, myData + 1, sizeof(T) * sizeMinusOne);
				// Push c style queue
				myData[sizeMinusOne] = aValue;
			}
			T* Get() noexcept
			{
				return myData;
			}
			T& operator[](size_t anIndex) noexcept
			{
				return myData[anIndex];
			}
			constexpr const int Size() const noexcept
			{
				return static_cast<int>(size);
			}
			constexpr const size_t SizeT() const noexcept
			{
				return size;
			}
		private:
			T myData[size];
			const size_t sizeMinusOne = size - 1;
		};
	}
}