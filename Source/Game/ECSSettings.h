#pragma once
#include <cstdint>
#include <bitset>

typedef std::uint32_t EntityType;
typedef EntityType Entity;

// Maximum amount of entities in play
#define MAX_ENTITIES 4096
// Amount of different Components in code
#define MAX_COMPONENTS 32

using ComponentID = std::uint8_t;
using SystemID = std::uint8_t;
using Signature = std::bitset<MAX_COMPONENTS>;

namespace ECS
{
    extern ComponentID globalUniqueComponentID;
    extern SystemID globalUniqueSystemID;

    template <class T>
    ComponentID GetComponentID()
    {
        static ComponentID componentId = globalUniqueComponentID++;
        return componentId;
    }
    template <class T>
    SystemID GetSystemID()
    {
        static SystemID systemId = globalUniqueSystemID++;
        return systemId;
    }
    template <class Component>
    Signature GetSignature()
    {
        return GetComponentID<Component>();
    }
    template <class Component1, class Component2, class... ComponentRest>
    Signature GetSignature()
    {
        return GetSignature<Component1>() | GetSignature<Component2, ComponentRest...>();
    }
}
