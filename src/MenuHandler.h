#pragma once

namespace MenuTracker
{
	bool is_any_menu_opened = false;
	bool is_magic_menu_opened = false;
}

class MenuHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		if (!a_event) {
			return RE::BSEventNotifyControl::kContinue;
		}
		MenuTracker::is_any_menu_opened = a_event->opening;
		if (a_event->menuName == RE::MagicMenu::MENU_NAME) {
			MenuTracker::is_magic_menu_opened = a_event->opening;
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	static bool Register()
	{
		static MenuHandler singleton;

		auto ui = RE::UI::GetSingleton();
		if (!ui) {
			logger::error("UI event source not found");
			return false;
		}

		ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(&singleton);
		
		logger::info("Registered {}", typeid(singleton).name());

		return true;
	}
};