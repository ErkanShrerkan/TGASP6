#pragma once

namespace SE {
	class CGameObject;
	class CComponent
	{
		friend class CGameObject;
	public:
		CComponent(CGameObject& aGameObject);
		virtual void Init() {};
		virtual void Update() {};
		virtual ~CComponent() {};
		CGameObject& GetParent() const;

	private:
		CGameObject& myParent;
	};
}
