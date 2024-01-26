#pragma once
namespace SE
{
    namespace Impl
    {
        typedef void HiddenIUnknown;
        namespace AutoReleaser
        {
            void Release(HiddenIUnknown* aResource);
        }
    }

    template <class T>
    class AutoReleaser
    {
    private:
        T* myResource;
    public:

        AutoReleaser()
            : myResource(nullptr)
        {}
        AutoReleaser(T* aResource)
            : myResource(aResource)
        {}

        ~AutoReleaser()
        {
            if (myResource)
            {
                #ifdef _DEBUG
                printf_s("AR: Releasing [%p] (%s)\n", myResource, typeid(T).name());
                #endif // _DEBUG
                SE::Impl::AutoReleaser::Release(myResource);
            }
        }

        inline void operator=(T* aPointer)
        {
            myResource = aPointer;
        }

        inline T*& Raw()
        {
            return myResource;
        }
        
        inline T*const& Raw() const noexcept
        {
            return myResource;
        }

        inline T*& operator->()
        {
            return myResource;
        }

        inline T** operator&()
        {
            return &myResource;
        }

        inline AutoReleaser<T>* GetAdress() const noexcept
        {
            return this;
        }

        inline constexpr bool operator==(T* anOtherPointer) const
        {
            return myResource == anOtherPointer;
        }

        inline constexpr bool operator!=(T* anOtherPointer) const
        {
            return myResource != anOtherPointer;
        }

        // Returns operator! on the pointer this object holds
        inline constexpr bool operator!() const
        {
            return !myResource;
        }
    };
}
