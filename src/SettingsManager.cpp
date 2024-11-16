#include <SimpleIni.h>
#include "SettingsManager.h"


void SettingsManager::ImportSettings() {
	std::lock_guard<std::shared_mutex> lk(settings_mtx);
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\SpellSwitch.ini");

	enable_input_switch = ini.GetBoolValue("Settings", "bEnableInputSwitch", false);
	concentration_delay = ini.GetDoubleValue("Settings", "fConcentrationDelay", 50.0);
	fire_forget_delay = ini.GetDoubleValue("Settings", "fFireForgetDelay", 500.0);
	interrupt_cast_delay = ini.GetDoubleValue("Settings", "fInterruptCastDelay", 50.0);
}