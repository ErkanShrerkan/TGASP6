#include "pch.h"
#include "JsonManager.h"
#include <ImGui/imgui.h>
#include "JsonDocument.h"

#define TREAT_AS(tam_type, tam_var) \
*reinterpret_cast<tam_type*>(tam_var)

JsonManager::JsonManager()
{
	myJsonDocument = nullptr;
}

JsonManager::~JsonManager()
{
	if (myJsonDocument)
		delete myJsonDocument;
	myJsonDocument = nullptr;
}

void JsonManager::InitDocument(const std::string& aConfigFilePath)
{
	myJsonDocument = new JsonDocument(aConfigFilePath);
	myConfigFile = aConfigFilePath;
}

void JsonManager::OnImGui()
{
	if (ImGui::BeginMenu("Exposed Variables"))
	{
		ImGui::BeginMenu(("Saving to \"" + myConfigFile + "\"").c_str(), false);

		for (auto& pair : myExposedVariables)
		{
			auto& category = pair.first;
			if (ImGui::BeginMenu(category.c_str()))
			{
				ImGui::Text("Save Settings");
				ImGui::SameLine();
				if (ImGui::Button("Save"))
				{
					for (auto& variable : pair.second)
					{
						AlterJsonValue(category, variable);
						myJsonDocument->SaveToFile(myConfigFile, true);
					}
				}
				ImGui::Separator();

				for (auto& variable : pair.second)
				{
					if (variable.address == nullptr)
						continue;

					const char* lid = variable.labelId.c_str();
					ImGui::Text("%s %s", variable.labelText.c_str(), variable.valueHasChanged ? "*" : "");

					switch (variable.type)
					{
					case JsonExposedType::Float1:
						if (ImGui::DragFloat(lid, reinterpret_cast<float*>(variable.address)))
						{
							for (auto& address : variable.addresses)
								TREAT_AS(float, address) = TREAT_AS(float, variable.address);
							variable.valueHasChanged = true;
						}
						break;
					case JsonExposedType::Float2:
						if (ImGui::DragFloat2(lid, reinterpret_cast<float*>(variable.address)))
						{
							for (auto& address : variable.addresses)
								TREAT_AS(float2, address) = TREAT_AS(float2, variable.address);
							variable.valueHasChanged = true;
						}
						break;
					case JsonExposedType::Float3:
						if (variable.imguiTag == JsonExposedImguiTag::ColorPicker)
						{
							if (ImGui::ColorEdit3(lid, reinterpret_cast<float*>(variable.address)))
							{
								for (auto& address : variable.addresses)
									TREAT_AS(float3, address) = TREAT_AS(float3, variable.address);
								variable.valueHasChanged = true;
							}
						}
						else
						{
							if (ImGui::DragFloat3(lid, reinterpret_cast<float*>(variable.address)))
							{
								for (auto& address : variable.addresses)
									TREAT_AS(float3, address) = TREAT_AS(float3, variable.address);
								variable.valueHasChanged = true;
							}
						}
						break;
					case JsonExposedType::Float4:

						if (variable.imguiTag == JsonExposedImguiTag::ColorPicker)
						{
							if (ImGui::ColorEdit4(lid, reinterpret_cast<float*>(variable.address)))
							{
								for (auto& address : variable.addresses)
									TREAT_AS(float4, address) = TREAT_AS(float4, variable.address);
								variable.valueHasChanged = true;
							}
						}
						else
						{
							if (ImGui::DragFloat4(lid, reinterpret_cast<float*>(variable.address)))
							{
								for (auto& address : variable.addresses)
									TREAT_AS(float4, address) = TREAT_AS(float4, variable.address);
								variable.valueHasChanged = true;
							}
						}
						break;
					case JsonExposedType::Bool:
						if (ImGui::Checkbox(lid, reinterpret_cast<bool*>(variable.address)))
						{
							for (auto& address : variable.addresses)
								TREAT_AS(bool, address) = TREAT_AS(bool, variable.address);
							variable.valueHasChanged = true;
						}
						break;
					default:
						break;
					}

					if (ImGui::BeginPopupContextItem(variable.labelId.c_str()))
					{
						if (ImGui::Button("Save"))
						{
							AlterJsonValue(category, variable);
							myJsonDocument->SaveToFile(myConfigFile, true);
						}
						if (ImGui::Button("Restore"))
						{
							RestoreValue(variable);
						}
						ImGui::EndPopup();
					}
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenu();
	}
}

void JsonManager::Unexpose(void* anAddres)
{
	(void)anAddres;
	for (auto& pair1 : myExposedVariables)
	{
		auto& vector = pair1.second;
		for (int i = static_cast<int>(vector.size()) - 1; i >= 0; i--)
		{
			auto& variable = pair1.second[i];
			if (variable.addresses.find(anAddres) != variable.addresses.end())
			{
				variable.addresses.erase(anAddres);
			}
			if (variable.addresses.size() == 0)
			{
				variable.address = nullptr;
			}
			else if (variable.address == anAddres)
			{
				variable.address = *variable.addresses.begin();
			}
		}
	}
}

void JsonManager::ExposeVariable(const std::string& aCategory, JsonExposedVariable& aVariable)
{
	auto key = aVariable.labelId;
	if (mySyncedVariableExposeIndexMapper[aCategory].find(key) == mySyncedVariableExposeIndexMapper[aCategory].end())
	{
		// First time exposing this variable

		aVariable.addresses.insert(aVariable.address);

		size_t index = myExposedVariables[aCategory].size();
		myExposedVariables[aCategory].push_back(aVariable);
		mySyncedVariableExposeIndexMapper[aCategory][key] = index;
	}
	else
	{
		// Second/nth time exposing this variable

		size_t index = mySyncedVariableExposeIndexMapper.at(aCategory).at(key);
		auto& exposedVariable = myExposedVariables.at(aCategory).at(index);
		exposedVariable.addresses.insert(aVariable.address);
		if (exposedVariable.address == nullptr)
			exposedVariable.address = aVariable.address;
	}

	LoadJsonValue(aCategory, aVariable);
}

void JsonManager::ExposeVariable(void* anAddress, JsonExposedType aType, const std::string& aLabel, const std::string& anOverrideLabel, const std::string& aCategory, const JsonExposedImguiTag& anImGuiTag)
{
	JsonExposedVariable variable{};
	
	variable.address = anAddress;
	variable.type = aType;
	variable.labelText = anOverrideLabel.length() ? anOverrideLabel : aLabel;
	variable.labelId = GetLabelId(aLabel);
	variable.imguiTag = anImGuiTag;
	switch (variable.type)
	{
	case JsonExposedType::Float1:
		variable.savedValue.asFloat1 = *reinterpret_cast<float1*>(anAddress);
		break;
	case JsonExposedType::Float2:
		variable.savedValue.asFloat2 = *reinterpret_cast<float2*>(anAddress);
		break;
	case JsonExposedType::Float3:
		variable.savedValue.asFloat3 = *reinterpret_cast<float3*>(anAddress);
		break;
	case JsonExposedType::Float4:
		variable.savedValue.asFloat4 = *reinterpret_cast<float4*>(anAddress);
		break;
	case JsonExposedType::Bool:
		variable.savedValue.asBool = *reinterpret_cast<bool*>(anAddress);
		break;
	default:
		__assume(0);
	}

	ExposeVariable(aCategory, variable);
}

std::string JsonManager::GetLabelId(const std::string& aLabel)
{
	std::string id = "##";
	id += aLabel;
	id.erase(std::remove(id.begin(), id.end(), ' '), id.end());
	return id;
}

void JsonManager::RestoreValue(JsonExposedVariable& aVariable)
{
	aVariable.valueHasChanged = false;
	switch (aVariable.type)
	{
	case JsonExposedType::Float1:
		for (auto& address : aVariable.addresses)
			TREAT_AS(float1, address) = aVariable.savedValue.asFloat1;
		break;
	case JsonExposedType::Float2:
		for (auto& address : aVariable.addresses)
			TREAT_AS(float2, address) = aVariable.savedValue.asFloat2;
		break;
	case JsonExposedType::Float3:
		for (auto& address : aVariable.addresses)
			TREAT_AS(float3, address) = aVariable.savedValue.asFloat3;
		break;
	case JsonExposedType::Float4:
		for (auto& address : aVariable.addresses)
			TREAT_AS(float4, address) = aVariable.savedValue.asFloat4;
		break;
	case JsonExposedType::Bool:
		for (auto& address : aVariable.addresses)
			TREAT_AS(bool, address) = aVariable.savedValue.asBool;
		break;
	default:
		__assume(0);
	}
}

void JsonManager::AlterJsonValue(const std::string& aCategory, JsonExposedVariable& aVariable)
{
	if (aVariable.address == nullptr)
		return;

	std::string destination = "/" + aCategory;
	destination += "/" + aVariable.labelId.substr(2);

	aVariable.valueHasChanged = false;
	switch (aVariable.type)
	{
	case JsonExposedType::Float1:
	{
		auto value = *reinterpret_cast<float1*>(aVariable.address);
		myJsonDocument->SetFloat(destination, value);
		aVariable.savedValue.asFloat1 = value;
		break;
	}
	case JsonExposedType::Float2:
	{
		auto value = *reinterpret_cast<float2*>(aVariable.address);
		myJsonDocument->SetFloat(destination + "/x", value.x);
		myJsonDocument->SetFloat(destination + "/y", value.y);
		aVariable.savedValue.asFloat2 = value;
		break;
	}
	case JsonExposedType::Float3:
	{
		auto value = *reinterpret_cast<float3*>(aVariable.address);
		myJsonDocument->SetFloat(destination + "/x", value.x);
		myJsonDocument->SetFloat(destination + "/y", value.y);
		myJsonDocument->SetFloat(destination + "/z", value.z);
		aVariable.savedValue.asFloat3 = value;
		break;
	}
	case JsonExposedType::Float4:
	{
		auto value = *reinterpret_cast<float4*>(aVariable.address);
		myJsonDocument->SetFloat(destination + "/x", value.x);
		myJsonDocument->SetFloat(destination + "/y", value.y);
		myJsonDocument->SetFloat(destination + "/z", value.z);
		myJsonDocument->SetFloat(destination + "/w", value.w);
		aVariable.savedValue.asFloat4 = value;
		break;
	}
	case JsonExposedType::Bool:
	{
		auto value = *reinterpret_cast<bool*>(aVariable.address);
		myJsonDocument->SetBool(destination, value);
		aVariable.savedValue.asBool = value;
		break;
	}
	default:
		__assume(0);
		break;
	}
}

void JsonManager::LoadJsonValue(const std::string& aCategory, JsonExposedVariable& aVariable)
{
	std::string destination = "/" + aCategory;
	destination += "/" + aVariable.labelId.substr(2);

	// Load value
	switch (aVariable.type)
	{
	case JsonExposedType::Float1:
	{
		float* value = reinterpret_cast<float1*>(aVariable.address);
		(*value) = myJsonDocument->GetFloat(destination, aVariable.savedValue.asFloat1);
		break;
	}
	case JsonExposedType::Float2:
	{
		float2* value = reinterpret_cast<float2*>(aVariable.address);
		(*value).x = myJsonDocument->GetFloat(destination + "/x", aVariable.savedValue.asFloat2.x);
		(*value).y = myJsonDocument->GetFloat(destination + "/y", aVariable.savedValue.asFloat2.y);
		break;
	}
	case JsonExposedType::Float3:
	{
		float3* value = reinterpret_cast<float3*>(aVariable.address);
		(*value).x = myJsonDocument->GetFloat(destination + "/x", aVariable.savedValue.asFloat3.x);
		(*value).y = myJsonDocument->GetFloat(destination + "/y", aVariable.savedValue.asFloat3.y);
		(*value).z = myJsonDocument->GetFloat(destination + "/z", aVariable.savedValue.asFloat3.z);
		break;
	}
	case JsonExposedType::Float4:
	{
		float4* value = reinterpret_cast<float4*>(aVariable.address);
		(*value).x = myJsonDocument->GetFloat(destination + "/x", aVariable.savedValue.asFloat4.x);
		(*value).y = myJsonDocument->GetFloat(destination + "/y", aVariable.savedValue.asFloat4.y);
		(*value).z = myJsonDocument->GetFloat(destination + "/z", aVariable.savedValue.asFloat4.z);
		(*value).w = myJsonDocument->GetFloat(destination + "/w", aVariable.savedValue.asFloat4.w);
		break;
	}
	case JsonExposedType::Bool:
	{
		bool* value = reinterpret_cast<bool*>(aVariable.address);
		(*value) = myJsonDocument->GetBool(destination, aVariable.savedValue.asBool);
		break;
	}
	default:
		__assume(0);
	}
}

std::string JsonManager::SanitizeFileString(const std::string& aFilePath)
{
	std::string sanitize = aFilePath;

	size_t index = 0;

	index = sanitize.find_last_of('\\');
	if (index != std::string::npos)
		sanitize = sanitize.substr(index + 1);

	index = sanitize.find_last_of('.');
	if (index != std::string::npos)
		sanitize = sanitize.substr(0, index);

	return sanitize;
}

#undef TREAT_AS
