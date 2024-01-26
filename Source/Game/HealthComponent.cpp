#include "pch.h"
#include "HealthComponent.h"

HealthComponent::HealthComponent(SE::CGameObject& aParent)
    : SE::CComponent(aParent)
{
    myHealth = 0;
}

HealthComponent::~HealthComponent()
{
}

void HealthComponent::Init(int aHealthAmount)
{
    myHealth = aHealthAmount;
}

void HealthComponent::Update()
{
}

const int HealthComponent::GetHealth()
{
    return myHealth;
}

const bool HealthComponent::IsDead()
{
    return myHealth <= 0;
}
