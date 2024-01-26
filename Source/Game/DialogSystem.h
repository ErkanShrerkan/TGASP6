#pragma once
#include <vector>
#include <memory>

class Text;
class Dialog
{
    friend class DialogSystem;
private:
    std::vector<std::shared_ptr<Text>> myText;
    size_t myCurrentTextIndex = 0;

private:
    void DisplayTextAt(const size_t& anIndex, const Vector3f& aPosition);
    void HideText(const size_t& anIndex);

public:
    void AddText(const std::string& someText);
    const size_t Count() const
    {
        return myText.size();
    }
};

#include <string>
#include <unordered_map>
class DialogSystem
{
private:
    std::unordered_map<std::string, Dialog> myDialogs;
    Dialog* myActiveDialog;

public:
    void Init(const std::string& aDialogFile);

    void SetActiveDialog(const std::string& anID);
    void Update();
};
