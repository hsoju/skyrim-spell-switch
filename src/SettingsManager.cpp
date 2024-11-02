#include <SimpleIni.h>
#include "SettingsManager.h"


void SettingsManager::ImportSettings() {
	std::lock_guard<std::shared_mutex> lk(settings_mtx);
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\SpellSwitch.ini");

	enable_input_switch = ini.GetBoolValue("Settings", "bEnableInputSwitch", false);
}