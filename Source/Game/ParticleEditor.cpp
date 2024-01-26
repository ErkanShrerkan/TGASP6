#include "pch.h"
#include "ParticleEditor.h"
#include "JsonDocument.h"
#include <ImGui\imgui.h>
#include <rapidjson\pointer.h>
#include <Engine/ContentLoader.h>
#include <Engine/Engine.h>
#include <Engine/ParticleEmitter.h>
#include <Engine/Texture.h>

void ParticleEditor::Init(const std::string& aConfigFile)
{
    myDocument = new JsonDocument(aConfigFile);
    myConfigFile = aConfigFile;

    auto& particles = myDocument->GetDocument()["Particles"];
    for (auto it = particles.MemberBegin(); it != particles.MemberEnd(); ++it)
    {
        ParticlePreset preset;
        preset.name = it->value["Name"].GetString();
        preset.sprite = it->value["Sprite"]["Path"].GetString();
        preset.oldSprite = preset.sprite;

        auto* emitter = SE::CEngine::GetInstance()->GetContentLoader()->GetParticleFactory().GetParticleEmitter(preset.name);
        preset.settings = emitter->GetSettings();

        myParticlePresets.push_back(preset);
    }

    size_t size = myParticlePresets.size();
    if (size > 0)
    {
        mySelectedPresetIndex = 0;
        ParticlePreset& preset = myParticlePresets[mySelectedPresetIndex];
        UpdateCharBuffer(&myCharBuffer, preset.name);
        UpdateCharBuffer(&myPathBuffer, preset.sprite);
    }
}

void ParticleEditor::OnImGui()
{
#ifdef DEBUG


    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Particle Editor"))
        {
            if (ImGui::Button("Create Particle Preset"))
            {
                ParticlePreset preset;
                preset.name = "Generic";

                mySelectedPresetIndex = myParticlePresets.size();
                myParticlePresets.push_back(preset);
                const ParticlePreset& spreset = myParticlePresets[mySelectedPresetIndex];
                UpdateCharBuffer(&myCharBuffer, spreset.name);
                UpdateCharBuffer(&myPathBuffer, spreset.sprite);
            }
            if (ImGui::Button("Clone Current Preset"))
            {
                ParticlePreset preset = myParticlePresets[mySelectedPresetIndex];
                preset.name += "_Copy";

                mySelectedPresetIndex = myParticlePresets.size();
                myParticlePresets.push_back(preset);
                const ParticlePreset& spreset = myParticlePresets[mySelectedPresetIndex];
                UpdateCharBuffer(&myCharBuffer, spreset.name);
                UpdateCharBuffer(&myPathBuffer, spreset.sprite);
            }
            ImGui::Separator();

            if (myParticlePresets.size() > 0)
            {
                if (ImGui::BeginCombo("Particle", myParticlePresets[mySelectedPresetIndex].name.c_str()))
                {
                    for (size_t i = 0; i < myParticlePresets.size(); i++)
                    {
                        bool selected = mySelectedPresetIndex == i;
                        if (ImGui::Selectable(myParticlePresets[i].name.c_str(), selected))
                        {
                            mySelectedPresetIndex = i;
                            ParticlePreset& preset = myParticlePresets[mySelectedPresetIndex];
                            UpdateCharBuffer(&myCharBuffer, preset.name);
                            UpdateCharBuffer(&myPathBuffer, preset.sprite);
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Save"))
                {
                    auto& preset = myParticlePresets[mySelectedPresetIndex];

                    std::string source = "/Particles/";
                    source += preset.name;
                    source += "/";

                    rapidjson::Pointer((source + "Name").c_str()).Set(myDocument->GetDocument(), preset.name.c_str());
                    rapidjson::Pointer((source + "Sprite/Path").c_str()).Set(myDocument->GetDocument(), preset.sprite.c_str());
                    WriteParticleSettings(source, preset.settings);
        
                    myDocument->SaveToFile(myConfigFile, true);
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete"))
                {
                    auto& preset = myParticlePresets[mySelectedPresetIndex];
                    // Remove from JSON file
                    std::string source = "/Particles/";
                    source += preset.name;
                    rapidjson::Pointer(source.c_str()).Erase(myDocument->GetDocument());
                    myDocument->SaveToFile(myConfigFile, true);

                    // Remove from vector
                    myParticlePresets.erase(myParticlePresets.begin() + mySelectedPresetIndex);

                    --mySelectedPresetIndex;
                    if (mySelectedPresetIndex == SIZE_MAX)
                    {
                        mySelectedPresetIndex = 0;
                        ImGui::EndMenu();
                        ImGui::EndMainMenuBar();
                        return;
                    }
                    UpdateCharBuffer(&myCharBuffer, preset.name);
                    UpdateCharBuffer(&myPathBuffer, preset.sprite);
                }
                if (ImGui::Button("Refresh Particles"))
                {
                    SE::CParticleEmitter* emitter = SE::CEngine::GetInstance()->GetContentLoader()->GetParticleFactory().GetParticleEmitter(myCharBuffer);

                    auto& preset = myParticlePresets[mySelectedPresetIndex];
                    if (preset.sprite != preset.oldSprite)
                    {
                        preset.oldSprite = preset.sprite;
                        SE::CTexture* texture = SE::CEngine::GetInstance()->GetContentLoader()->GetTextureFactory().LoadTexture(preset.sprite);
                        if (texture->GetShaderResourceView() == nullptr)
                        {
                            SE::CTexture* error = SE::CEngine::GetInstance()->GetContentLoader()->GetTextureFactory().LoadTexture("Textures/Error/Albedo_c.dds");
                            emitter->SwitchShaderResourceView(error->GetShaderResourceView());
                        }
                        else
                        {
                            emitter->SwitchShaderResourceView(texture->GetShaderResourceView());
                        }
                    }

                    emitter->SetSettings(myParticlePresets[mySelectedPresetIndex].settings);
                }
                ImGui::Separator();

                auto& preset = myParticlePresets[mySelectedPresetIndex];

                if (ImGui::InputText("Name", myCharBuffer, 128))
                {
                    preset.name = myCharBuffer;
                }
                if (ImGui::TreeNodeEx("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::InputText("Path", myPathBuffer, 128))
                    {
                        preset.sprite = myPathBuffer;
                    }
                    ImGui::Checkbox("Is Animated", &preset.settings.spriteSheet.isAnimated);
                    ImGui::Checkbox("Start With Random Index", &preset.settings.spriteSheet.startWithRandomIndex);
                    if (ImGui::DragFloat("Amount", &preset.settings.spriteSheet.amount))
                    {
                        preset.settings.spriteSheet.size = 1.0f / preset.settings.spriteSheet.amount;
                    }
                    ImGui::DragFloat("Size (Normalized)", &preset.settings.spriteSheet.size);
                    ImGui::DragFloat("Interval (s)", &preset.settings.spriteSheet.interval);

                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Spawn Radius (cm)", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat("##ptcl_sar", &preset.settings.spawnRadius);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Spawn Rate (p/s) [Needs restart]", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat("##ptcl_sr", &preset.settings.spawnRate);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Lifetime (s)", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat("##ptcl_lif", &preset.settings.particle.lifeTime);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Speed (cm/s)", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat("Minimum##ptcl_spdmin", &preset.settings.particle.speed.minimum);
                    ImGui::DragFloat("Maximum##ptcl_spdmax", &preset.settings.particle.speed.maximum);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Gravity (cm/s^2)", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat("Minimum##ptcl_grvmin", &preset.settings.particle.gravity.minimum);
                    ImGui::DragFloat("Maximum##ptcl_grvmax", &preset.settings.particle.gravity.maximum);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Direction", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Text("x | y | z");
                    ImGui::DragFloat3("Minimum##ptcl_dirmin", &preset.settings.particle.direction.minimum.x);
                    ImGui::DragFloat3("Maximum##ptcl_dirmax", &preset.settings.particle.direction.maximum.x);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Size", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::TreeNodeEx("Start", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::DragFloat("Minimum##ptcl_sszmin", &preset.settings.particle.startSize.minimum);
                        ImGui::DragFloat("Maximum##ptcl_sszmax", &preset.settings.particle.startSize.maximum);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNodeEx("End", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::DragFloat("Minimum##ptcl_eszmin", &preset.settings.particle.endSize.minimum);
                        ImGui::DragFloat("Maximum##ptcl_eszmax", &preset.settings.particle.endSize.maximum);
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Color", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::TreeNodeEx("Start", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::ColorEdit4("Minimum##ptcl_colsmin", &preset.settings.particle.startColor.minimum.x, ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4("Maximum##ptcl_colsmax", &preset.settings.particle.startColor.maximum.x, ImGuiColorEditFlags_AlphaBar);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNodeEx("End", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::ColorEdit4("Minimum##ptcl_colemin", &preset.settings.particle.endColor.minimum.x, ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4("Maximum##ptcl_colemax", &preset.settings.particle.endColor.maximum.x, ImGuiColorEditFlags_AlphaBar);
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Emissive Strength", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat("Minimum##ptcl_emsmin", &preset.settings.particle.emissiveStrength.minimum);
                    ImGui::DragFloat("Maximum##ptcl_emsmax", &preset.settings.particle.emissiveStrength.maximum);
                    ImGui::TreePop();
                }
            }
            else
            {
                ImGui::Text("There exist no particle presets. Create a new one to get started.");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
#endif // DEBUG
}

void ParticleEditor::UpdateCharBuffer(char (*aBuffer)[128], const std::string& aString)
{
    auto buffer = *aBuffer;
    for (size_t i = 0; i < aString.length(); i++)
    {
        buffer[i] = aString[i];
    }
    for (size_t i = aString.length(); i < 128; i++)
    {
        buffer[i] = '\0';
    }
}

void ParticleEditor::WriteParticleSettings(const std::string& aSource, SE::CParticleEmitter::SParticleSettings& someSettings)
{
    myDocument->SetFloat(aSource + "SpawnRate",         someSettings.spawnRate);
    myDocument->SetFloat(aSource + "SpawnRadius",       someSettings.spawnRadius);
    myDocument->SetFloat(aSource + "Lifetime",          someSettings.particle.lifeTime);
    myDocument->SetFloat(aSource + "Speed/Min",         someSettings.particle.speed.minimum);
    myDocument->SetFloat(aSource + "Speed/Max",         someSettings.particle.speed.maximum);
    myDocument->SetFloat(aSource + "Size/Start/Min",    someSettings.particle.startSize.minimum);
    myDocument->SetFloat(aSource + "Size/Start/Max",    someSettings.particle.startSize.maximum);
    myDocument->SetFloat(aSource + "Size/End/Min",      someSettings.particle.endSize.minimum);
    myDocument->SetFloat(aSource + "Size/End/Max",      someSettings.particle.endSize.maximum);
    myDocument->SetFloat(aSource + "Emissive/Min",      someSettings.particle.emissiveStrength.minimum);
    myDocument->SetFloat(aSource + "Emissive/Max",      someSettings.particle.emissiveStrength.maximum);
    myDocument->SetFloat(aSource + "Gravity/Min",       someSettings.particle.gravity.minimum);
    myDocument->SetFloat(aSource + "Gravity/Max",       someSettings.particle.gravity.maximum);
    myDocument->SetFloat(aSource + "Direction/Min/x",   someSettings.particle.direction.minimum.x);
    myDocument->SetFloat(aSource + "Direction/Max/x",   someSettings.particle.direction.maximum.x);
    myDocument->SetFloat(aSource + "Direction/Min/y",   someSettings.particle.direction.minimum.y);
    myDocument->SetFloat(aSource + "Direction/Max/y",   someSettings.particle.direction.maximum.y);
    myDocument->SetFloat(aSource + "Direction/Min/z",   someSettings.particle.direction.minimum.z);
    myDocument->SetFloat(aSource + "Direction/Max/z",   someSettings.particle.direction.maximum.z);
    myDocument->SetFloat(aSource + "Color/Start/Min/x", someSettings.particle.startColor.minimum.x);
    myDocument->SetFloat(aSource + "Color/Start/Max/x", someSettings.particle.startColor.maximum.x);
    myDocument->SetFloat(aSource + "Color/Start/Min/y", someSettings.particle.startColor.minimum.y);
    myDocument->SetFloat(aSource + "Color/Start/Max/y", someSettings.particle.startColor.maximum.y);
    myDocument->SetFloat(aSource + "Color/Start/Min/z", someSettings.particle.startColor.minimum.z);
    myDocument->SetFloat(aSource + "Color/Start/Max/z", someSettings.particle.startColor.maximum.z);
    myDocument->SetFloat(aSource + "Color/Start/Min/w", someSettings.particle.startColor.minimum.w);
    myDocument->SetFloat(aSource + "Color/Start/Max/w", someSettings.particle.startColor.maximum.w);
    myDocument->SetFloat(aSource + "Color/End/Min/x",   someSettings.particle.endColor.minimum.x);
    myDocument->SetFloat(aSource + "Color/End/Max/x",   someSettings.particle.endColor.maximum.x);
    myDocument->SetFloat(aSource + "Color/End/Min/y",   someSettings.particle.endColor.minimum.y);
    myDocument->SetFloat(aSource + "Color/End/Max/y",   someSettings.particle.endColor.maximum.y);
    myDocument->SetFloat(aSource + "Color/End/Min/z",   someSettings.particle.endColor.minimum.z);
    myDocument->SetFloat(aSource + "Color/End/Max/z",   someSettings.particle.endColor.maximum.z);
    myDocument->SetFloat(aSource + "Color/End/Min/w",   someSettings.particle.endColor.minimum.w);
    myDocument->SetFloat(aSource + "Color/End/Max/w",   someSettings.particle.endColor.maximum.w);
    // Sprite
    myDocument->SetFloat(aSource + "Sprite/Size",                 someSettings.spriteSheet.size);
    myDocument->SetFloat(aSource + "Sprite/Amount",               someSettings.spriteSheet.amount);
    myDocument->SetFloat(aSource + "Sprite/Interval",             someSettings.spriteSheet.interval);
    myDocument->SetBool(aSource +  "Sprite/IsAnimated",           someSettings.spriteSheet.isAnimated);
    myDocument->SetBool(aSource +  "Sprite/StartWithRandomIndex", someSettings.spriteSheet.startWithRandomIndex);
}
