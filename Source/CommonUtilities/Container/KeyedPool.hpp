#pragma once
#include <functional>
#include <map>
namespace CommonUtilities
{
    template <class Key, class Object>
    class KeyedPool
    {
    public:
        ~KeyedPool() 
        {
            for (auto& [key, obj] : myObjects)
            {
                delete obj;
            }
            myObjects.clear();
        }

        Object* Get(const Key& aKey, std::function<Object*(const Key&)> aCreateMethod)
        {
            auto position = myObjects.find(aKey);
            if (position != myObjects.end())
            {
                return position->second;
            }
            else
            {
                Object* object = aCreateMethod(aKey);
                myObjects.insert({ aKey, object });
                return object;
            }
        }

    private:
        std::map<Key, Object*> myObjects;
    };
}

#define CU_KEYED_POOL_LAMBDA(aKey, aKeyParameter, anObject, anObjectFunction) \
[this, aKeyParameter](const aKey&) -> anObject* { \
    return anObjectFunction(aKeyParameter); \
}
