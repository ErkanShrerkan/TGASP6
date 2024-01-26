#pragma once

namespace SE
{
	class CHealth
	{
		public:
			void Init(float someMaxHealth);
			void SetMaxHealth(float anAmount);
			void SetHealth(float anAmount);
			void TakeDamage(float anAmount);
			void Heal(float anAmount);
			void ResetHealth();

			float &GetHealth();
			float GetMaxHealth();
			bool IsEntityDead();

		private:
			float myHealth;
			float myMaxHealth;
	};
}


