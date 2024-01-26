#pragma once
// Purpose: Contain components
#include <map>
#include <vector>
#include <typeinfo>
#include <string_view>
#include "Component.h"
#include "Transform.h"
namespace SE
{
	class CGameObject
	{
	#pragma region Component
	public:

		template <class T>
		T* AddComponent(const std::string_view& aName);
		template <class T>
		T* AddComponent();

		template <class T>
		T* GetComponent(size_t anIndex);
		template <class T>
		T* GetComponent(const std::string_view& aName);
		template <class T>
		T* GetComponent();

	private:
		using HashCode = size_t;
		using Index = size_t;

		// TODO: Consider using CommonUtilities::MemoryBlock

		std::map<HashCode, std::vector<CComponent*>> myComponents;
		std::map<std::string_view, std::pair<HashCode, Index>> myNamedComponents;
	#pragma endregion
	#pragma region General

	public:
		void Init();
		void Update();
		bool IsMarkedForRemoval() const;
		void MarkForRemoval();

		void SetTransform(const Vector3f& aPosition, const Vector3f& aRotation);
		void SetTransform(const Matrix4x4f& aTransform);
		Matrix4x4f& GetTransform();
		void SetRotation(const Vector3f& aRotation);
		void SetPosition(const Vector3f& aPosition);
		const Vector3f GetRotation() const;
		const Vector3f GetPosition() const;
		void Move(const Vector3f& aMovement);
		void Rotate(const Vector3f& aRotation);
		const Vector3f& GetScale() const noexcept;
		void SetScale(const Vector3f& aScale);

	private:
		bool myIsMarkedForRemoval = false;
		CTransform myTransform;
		Vector3f myScale = Vector3f::One();
	#pragma endregion
	};

	template<class T>
	inline T* CGameObject::AddComponent(const std::string_view& aName)
	{
		static_assert(std::is_base_of<CComponent, T>::value, "Component must derive of SE::CComponent");

		T* component = new T(*this);

		const HashCode hash = typeid(T).hash_code();
		auto& components = myComponents[hash];
		std::string name = std::string(aName);
		if (name.empty())
		{
			name = std::to_string(reinterpret_cast<size_t>(component));
		}
		auto& nmpr = myNamedComponents[name];
		nmpr.first = hash;
		nmpr.second = components.size();

		components.push_back(component);
		return component;
	}

	template<class T>
	inline T* CGameObject::AddComponent()
	{
		return AddComponent<T>("");
	}

	template<class T>
	inline T* CGameObject::GetComponent(size_t anIndex)
	{
		return reinterpret_cast<T*>(myComponents.at(typeid(T).hash_code()).at(anIndex));
	}

	template<class T>
	inline T* CGameObject::GetComponent(const std::string_view& aName)
	{
		HashCode hash = myNamedComponents.at(aName).first;
		Index index = myNamedComponents.at(aName).second;
		return reinterpret_cast<T*>(myComponents.at(hash).at(index));
	}

	template<class T>
	inline T* CGameObject::GetComponent()
	{
		return GetComponent<T>(0);
	}
}
