#pragma once

#include "AnimationEventHandler.h"
#include "MenuHandler.h"
#include "SwitchManager.h"
#include "SettingsManager.h"

class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
{
public:
	AnimationEventHandler* animation_handler = AnimationEventHandler::GetSingleton();
	SwitchManager* switch_manager = SwitchManager::GetSingleton();
	SettingsManager* settings_manager = SettingsManager::GetSingleton();

	void ProcessFavoritesInput(RE::InputEvent* const* a_event);
	void ProcessToggleInput(RE::InputEvent* const* a_event);

	virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
	{
		if (settings_manager->enable_input_switch && !MenuTracker::is_any_menu_opened) {
			if (!a_event) {
				return RE::BSEventNotifyControl::kContinue;
			}
			ProcessToggleInput(a_event);
		}
		if (MenuTracker::is_magic_menu_opened) {
			if (!a_event) {
				return RE::BSEventNotifyControl::kContinue;
			}
			ProcessFavoritesInput(a_event);
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	static bool Register()
	{
		static InputEventHandler singleton;

		auto input_manager = RE::BSInputDeviceManager::GetSingleton();
		if (!input_manager) {
			logger::error("Input event source not found");
			return false;
		}
		input_manager->AddEventSink(&singleton);
		logger::info("Registered {}", typeid(singleton).name());
		
		return true;
	}
};