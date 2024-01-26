#pragma once
#include <vector>

namespace SE
{
    class CGameObject;
}

// Antipattern?
class ScenePopulator
{
public:
    bool PopulateVectorFromJson(std::vector<SE::CGameObject>& aVectorToFill, const std::string_view& aFilePath);

private:

};