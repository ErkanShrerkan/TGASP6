#pragma once
class AttackPower
{
	public:
		void Init(float someMaxAttackPower);
		void SetMaxAttackPower(float anAmount);
		void DrainAttackPower(float anAmount);
		void RegainAttackPower(float anAmount);
		void EmptyAttackPower();
		void ResetAttackPower();


		float& GetAttackPower();
		float& GetMaxAttackPower();

	private:
		float myAttackPower;
		float myMaxAttackPower;
};

