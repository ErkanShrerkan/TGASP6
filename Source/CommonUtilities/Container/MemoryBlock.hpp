#pragma once
#include <new>
namespace CommonUtilities
{
    class MemoryBlock
    {
        using Byte = unsigned char;
    public:

        const constexpr size_t Size() const
        {
            return mySize;
        }

        void ReserveSpace(size_t aSize)
        {
            mySize += aSize;
        }

        template <class T>
        void ReserveSpace()
        {
            ReserveSpace(sizeof(T));
        }

        void AllocateMemory()
        {
            myArray = new Byte[mySize];
        }

        void AlignInMemory(size_t* anOffset, void** anAddress, size_t aSize)
        {
            size_t offset = *anOffset;
            *anOffset += aSize;
            *anAddress = reinterpret_cast<void*>(myArray + offset);
        }

        template <class T>
        void AlignInMemory(size_t* anOffset, T** anAddress)
        {
            AlignInMemory(anOffset, reinterpret_cast<void**>(anAddress), sizeof(T));
        }

        ~MemoryBlock()
        {
            delete myArray;
        }

    private:
        Byte* myArray = nullptr;
        size_t mySize = 0;
    };
}
