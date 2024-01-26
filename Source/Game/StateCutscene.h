#pragma once
#include "State.h"
#include <map>
#include <Engine/Camera.h>
#include "Transform.h"
#include <vector>
#include <CommonUtilities/Math/Quaternion.h>

namespace SE
{
    class CScene;
}
enum class eCutscenes
{
    None,
    Intro,
    MinibossIntro,
    MinibossEnd,
    BossIntro,
    NpcDialog,
    BossDead,
    MinibossPortalOpen,
    Count
};
class UIElement;
class UIDialog;
class Stopwatch;
class StateCutscene :
    public State
{
public:
    StateCutscene();
    ~StateCutscene();
    bool Update() override;
    void Render() override;
    void PlayCutscene(eCutscenes aCutscene);
    void AddCutsceneKeyframe(eCutscenes aCutscene, Transform aTransform);
    void Reset();
    

private:
    void UpdateCutscene();
    void DeactivateBlackbars();
    void ActivateCutscene();
    void PlayIntroFade();
    void PlayOutroFade();
    void BlackscreenUpdate();
    void ReverseBlackscreenUpdate();
    void BeginEnding();

    std::map<eCutscenes, std::vector<Quaternion>> myCutsceneCameraQuaternions;
    std::map<eCutscenes, std::vector<Vector3f>> myCutsceneCameraPositions;
    std::map<eCutscenes, std::vector<UIDialog*>> myCutsceneDialogs;
private:
    Quaternion mySavedCameraQuaternion;
    Quaternion myCurrentCameraQuaternion;
    Vector3f mySavedCameraPosition;
    Vector3f myCurrentCameraPosition;
    bool myActiveCutscene = false;
    bool myActiveBlackscreen = false;
    bool myReverseBlackscreen = false;
    bool myCreditsStandstill = true;
    bool myShouldUpdateCredits = false;
    bool myIsEnding = false;

    int myKeyframeIndex = 0;

    UIElement* myBlackbarUp;
    UIElement* myBlackbarDown;
    UIElement* myBlackscreen;
    UIElement* myCredits;
    Stopwatch* myCutsceneStopwatch;
    Stopwatch* myBlackbarsStopwatch;
    Stopwatch* myBlackscreenStopwatch;
    Stopwatch* mySecondBlackscreenStopwatch;
    Stopwatch* myCreditsStopwatch;
    Stopwatch* myStartCreditsWait;
    Stopwatch* myCreditsDoneStopwatch;
    SE::CCamera* myCamera;
    eCutscenes myCurrentCutscene;
};

