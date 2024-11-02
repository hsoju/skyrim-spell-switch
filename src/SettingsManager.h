#pragma once
#include <shared_mutex>

class SettingsManager
{
private:
	std::shared_mutex settings_mtx;	

public:
	bool enable_input_switch = false;

	static SettingsManager* GetSingleton() {
		static SettingsManager singleton;
		return &singleton;
	}

	void ImportSettings();

protected:
	SettingsManager() = default;
	SettingsManager(const SettingsManager&) = delete;
	SettingsManager(SettingsManager&&) = delete;
	virtual ~SettingsManager() = default;

	auto operator=(const SettingsManager&) -> SettingsManager& = delete;
	auto operator=(SettingsManager&&) -> SettingsManager& = delete;
};