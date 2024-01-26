#pragma once
#include <vector>
#include <assert.h>


namespace CommonUtilities
{
	template <class T>
	class Heap
	{
	public:
		int GetSize() const
		{
			return (int)myData.size();
		}
		void Enqueue(const T& aElement)
		{
			int index = GetSize();
			myData.push_back(aElement);

			int parentIndex = (index - 1) / 2;

			while (parentIndex >= 0 && parentIndex != index && myData[index] < myData[parentIndex])
			{
				T temp = myData[parentIndex];
				myData[parentIndex] = myData[index];
				myData[index] = temp;

				index = parentIndex;
				parentIndex = (index - 1) / 2;
			}
		}
		const T& GetTop() const
		{
			assert(myData.size());

			return myData[0];
		}
		T Dequeue()
		{
			assert(myData.size());

			T top = myData[0];
			myData[0] = myData[GetSize() - 1];
			myData.pop_back();

			int index = 0;
			int childIndex = 0;

			do
			{
				index = childIndex;

				const int childIndex1 = 2 * index + 1;
				const int childIndex2 = 2 * index + 2;
				const int size = GetSize();

				if (childIndex2 < size)
				{
					childIndex = myData[childIndex1] < myData[childIndex2] ? childIndex1 : childIndex2;
				}
				else if (childIndex1 < size)
				{
					childIndex = childIndex1;
				}
				else break;

				if (!(myData[childIndex] < myData[index]))
				{
					break;
				}

				T temp = myData[childIndex];
				myData[childIndex] = myData[index];
				myData[index] = temp;
			} while (childIndex != index);

			return top;
		}
	private:
		std::vector<T> myData;
	};
}