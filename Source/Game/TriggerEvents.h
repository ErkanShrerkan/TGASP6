#pragma once

enum class TriggerEvent
{
    None,
#pragma region TriggerNames

    Player_Ability_AOE,
    Player_Ability_RmbAttack,
    Player_Ability_Dammsugare,
    Player_Ability_Lmb,
    Player_Ability_Ultimate,
    Player_Ability_Teleport,
    LightEnemy_Ability_BasicAttack,
    CultistEnemy_Ability_Ranged,
    Boss_FirstAttack,
    Boss_SecondAttack,
    Boss_ThirdAttack,
    Level_1,
    Level_2,
    Level_3,
    Level_4,
    Level_5,
    SpawnBoss,
    MinibossCutscene,
    DialogCutscene,

#pragma endregion
    // Keep last, don't delete
    Count
};
