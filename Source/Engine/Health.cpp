#include "pch.h"
#include "Health.h"


namespace SE 
{
	void CHealth::Init(float someMaxHealth)
	{
		myMaxHealth = someMaxHealth;
		myHealth = myMaxHealth;
	}
	void SE::CHealth::SetMaxHealth(float anAmount)
	{
		myMaxHealth = anAmount;
	}

	void SE::CHealth::SetHealth(float anAmount)
	{
		myHealth = anAmount;
	}

	void SE::CHealth::TakeDamage(float anAmount)
	{
		myHealth -= anAmount;

		if(myHealth < 0)
		{
			myHealth = 0;
		}
	}

	void SE::CHealth::Heal(float anAmount)
	{
		myHealth += anAmount;

		if(myHealth > myMaxHealth)
		{
			myHealth = myMaxHealth;
		}
	}

	void SE::CHealth::ResetHealth()
	{
		myHealth = myMaxHealth;
	}

	float& SE::CHealth::GetHealth()
	{
		return myHealth;
	}

	float SE::CHealth::GetMaxHealth()
	{
		return myMaxHealth;
	}

	bool SE::CHealth::IsEntityDead()
	{
		return myHealth <= 0;
	}

}
