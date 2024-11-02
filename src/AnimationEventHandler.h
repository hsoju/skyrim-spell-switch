#pragma once

class AnimationEventHandler
{
public:
	static AnimationEventHandler* GetSingleton()
	{
		static AnimationEventHandler singleton;
		return &singleton;
	}

	std::string right_cast_begin = "BeginCastRight";
	std::string left_cast_begin = "BeginCastLeft";

	bool has_casted_right = false;
	bool has_casted_left = false;
	bool has_dualcast = false;

	int num_right_casts = 0;
	int num_left_casts = 0;

	RE::TESForm* current_right_spell = nullptr;
	RE::TESForm* current_left_spell = nullptr;

	//std::string right_cast_firing = "MRh_SpellFire_Event";
	//std::string left_cast_firing = "MLh_SpellFire_Event";

	std::string right_cast_fire_forget = "MRh_WinStart";
	std::string left_cast_fire_forget = "MLh_WinStart";

	bool has_fired_right = false;
	bool has_fired_left = false;

	std::string right_concentration_cast = "MRh_PreAimedConcentrationOut";
	std::string right_concentration_cast_self = "MRh_PreSelfConOut";
	std::unordered_set<std::string> right_concentration_casts = { right_concentration_cast, right_concentration_cast_self };

	std::string left_concentration_cast = "MLh_PreChargeOut";
	std::string left_concentration_cast_self = "MLh_PreSelfConOut";
	std::unordered_set<std::string> left_concentration_casts = { left_concentration_cast, left_concentration_cast_self };

	bool is_concentrating_right = false;
	bool is_concentrating_left = false;

	std::string stop_cast = "CastStop";

	std::unordered_set<std::string> valid_events = { right_cast_begin, left_cast_begin, // right_cast_firing, left_cast_firing, 
		right_cast_fire_forget, left_cast_fire_forget, right_concentration_cast, right_concentration_cast_self,
		left_concentration_cast, left_concentration_cast_self, stop_cast };

	void ResetHasCast(bool use_left_hand);

	void ToggleSpell(bool use_left_hand);
	void SwitchSpell(bool use_left_hand, int num_casts);
	
	void CheckInitialRightCast(RE::TESForm* next_spell, RE::SpellItem* spell_object);
	void CheckInitialLeftCast(RE::TESForm* next_spell, RE::SpellItem* spell_object);
	void CheckInitialCast(bool use_left_hand);

	void ScheduleRightSwitch();
	void ScheduleLeftSwitch();
	void ScheduleBothSwitches();

	void CheckFireForgetRightSwitch();
	void CheckFireForgetLeftSwitch();
	void CheckFireForgetSwitch(bool use_left_hand);
	void CheckConcentrationSwitch();

	void CheckAnimation(std::string& animation_event);

protected:
	AnimationEventHandler() = default;
	AnimationEventHandler(const AnimationEventHandler&) = delete;
	AnimationEventHandler(AnimationEventHandler&&) = delete;
	virtual ~AnimationEventHandler() = default;

	auto operator=(const AnimationEventHandler&) -> AnimationEventHandler& = delete;
	auto operator=(AnimationEventHandler&&) -> AnimationEventHandler& = delete;
};