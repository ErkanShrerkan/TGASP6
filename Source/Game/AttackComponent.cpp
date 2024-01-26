#include "pch.h"
#include "AttackComponent.h"

AttackComponent::AttackComponent(SE::CGameObject& aParent)
	: SE::CComponent(aParent)
{
	myAttackValue = 0;
	myCriticalStrikeChance = 0;
}

AttackComponent::~AttackComponent()
{
}

void AttackComponent::Update()
{
}

void AttackComponent::Init(int aAttackValue)
{
	myAttackValue = aAttackValue;
	myCriticalStrikeChance = 20;
}

void AttackComponent::Attack()
{
}
