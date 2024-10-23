#pragma once

#include "MenuHandler.h"
#include "SwitchManager.h"


class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
{
public:
	SwitchManager* manager = SwitchManager::GetSingleton();

	virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
	{
		if (MagicMenuTracker::is_menu_opened) {
			if (!a_event) {
				return RE::BSEventNotifyControl::kContinue;
			}
			for (RE::InputEvent* given_input = *a_event; given_input; given_input = given_input->next) {
				if (given_input->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) {
					continue;
				}
				const RE::ButtonEvent* given_button = given_input->AsButtonEvent();
				if (given_button->IsPressed() || given_button->IsHeld()) {
					continue;
				}

				const RE::IDEvent* given_id = given_input->AsIDEvent();
				const auto& given_user = given_id->userEvent;
				const auto user_events = RE::UserEvents::GetSingleton();

				if (given_user == user_events->toggleFavorite || given_user == user_events->yButton) {
					manager->ProcessFavoriteSpells();
					break;
				}
			}
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