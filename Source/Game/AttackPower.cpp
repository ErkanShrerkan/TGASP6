#include "pch.h"
#include "AttackPower.h"

void AttackPower::Init(float someMaxAttackPower)
{
	myMaxAttackPower = someMaxAttackPower;
	myAttackPower = myMaxAttackPower;
}

void AttackPower::SetMaxAttackPower(float anAmount)
{
	myMaxAttackPower = anAmount;
}

void AttackPower::DrainAttackPower(float anAmount)
{
	myAttackPower -= anAmount;

	if(myAttackPower < 0)
	{
		myAttackPower = 0;
	}
}

void AttackPower::RegainAttackPower(float anAmount)
{
	myAttackPower += anAmount;

	if(myAttackPower > myMaxAttackPower)
	{
		myAttackPower = myMaxAttackPower;
	}
}

void AttackPower::EmptyAttackPower()
{
	myAttackPower = 0;
}

void AttackPower::ResetAttackPower()
{
	myAttackPower = myMaxAttackPower;
}

float& AttackPower::GetAttackPower()
{
	return myAttackPower;
}

float& AttackPower::GetMaxAttackPower()
{
	return myMaxAttackPower;
}
