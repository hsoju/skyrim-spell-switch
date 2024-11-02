#include "AnimationEventHandler.h"
#include "AsyncManager.h"
#include "SwitchManager.h"


RE::BGSEquipSlot* GetLeftHandSlot()
{
	using func_t = decltype(&GetLeftHandSlot);
	const REL::Relocation<func_t> func{ RELOCATION_ID(23150, 23607) };
	return func();
}

RE::BGSEquipSlot* GetRightHandSlot()
{
	using func_t = decltype(&GetRightHandSlot);
	const REL::Relocation<func_t> func{ RELOCATION_ID(23151, 23608) };
	return func();
}

void AnimationEventHandler::ResetHasCast(bool use_left_hand) {
	if (use_left_hand) {
		has_casted_left = false;
	} else {
		has_casted_right = false;
	}
}

void AnimationEventHandler::ToggleSpell(bool use_left_hand)
{
	auto player = RE::PlayerCharacter::GetSingleton();
	if (player->IsInCombat()) {
		auto state = player->AsActorState();
		if (state && state->IsWeaponDrawn()) {
			RE::TESForm* equipped_item = player->GetEquippedObject(use_left_hand);
			if (equipped_item && equipped_item->GetFormType() == RE::FormType::Spell) {
				if (use_left_hand) {
					current_left_spell = equipped_item;
				} else {
					current_right_spell = equipped_item;
				}
				SwitchSpell(use_left_hand, 1);
			}
		}
	}
}

void AnimationEventHandler::SwitchSpell(bool use_left_hand, int num_casts)
{
	auto player = RE::PlayerCharacter::GetSingleton();
	bool is_left_casting;
	bool is_right_casting;
	player->GetGraphVariableBool("bWantCastLeft", is_left_casting);
	player->GetGraphVariableBool("bWantCastRight", is_right_casting);
	if (!is_left_casting && !is_right_casting) {
		ResetHasCast(use_left_hand);
		RE::TESForm* equipped_item = player->GetEquippedObject(use_left_hand);
		RE::TESForm* spell_to_check = (use_left_hand) ? current_left_spell : current_right_spell;
		if (equipped_item == spell_to_check) {
			RE::SpellItem* equipped_spell = equipped_item->As<RE::SpellItem>();
			RE::SpellItem* switched_spell = SwitchManager::GetSingleton()->GetNextSpell(equipped_spell, use_left_hand, num_casts);
			if (equipped_spell != switched_spell) {
				if (use_left_hand) {
					RE::ActorEquipManager::GetSingleton()->EquipSpell(player, switched_spell, GetLeftHandSlot());
				} else {
					RE::ActorEquipManager::GetSingleton()->EquipSpell(player, switched_spell, GetRightHandSlot());
				}
			}
			if (!use_left_hand) {
				num_right_casts = 0;
			} else {
				num_left_casts = 0;
			}
		}
		//logger::info("Current spell = {}, Next switch = {}", equipped_spell->fullName.data(), switched_spell->fullName.data());
	}
}

void AnimationEventHandler::CheckInitialRightCast(RE::TESForm* next_spell, RE::SpellItem* spell_object) {
	if (!current_right_spell || current_right_spell != next_spell) {
		num_right_casts = 1;
	} else {
		num_right_casts += 1;
	}
	current_right_spell = next_spell;
	is_concentrating_right = spell_object->GetCastingType() == RE::MagicSystem::CastingType::kConcentration;
	has_casted_right = true;
	has_fired_right = false;
}

void AnimationEventHandler::CheckInitialLeftCast(RE::TESForm* next_spell, RE::SpellItem* spell_object) {
	if (!current_left_spell || current_left_spell != next_spell) {
		num_left_casts = 1;
	} else {
		num_left_casts += 1;
	}
	current_left_spell = next_spell;
	is_concentrating_left = spell_object->GetCastingType() == RE::MagicSystem::CastingType::kConcentration;
	has_casted_left = true;
	has_fired_left = false;
}

void AnimationEventHandler::CheckInitialCast(bool use_left_hand) {
	auto next_spell = RE::PlayerCharacter::GetSingleton()->GetEquippedObject(use_left_hand);
	if (next_spell->GetFormType() == RE::FormType::Spell) {
		auto spell_object = next_spell->As<RE::SpellItem>();
		if (use_left_hand) {
			CheckInitialLeftCast(next_spell, spell_object);
		} else {
			CheckInitialRightCast(next_spell, spell_object);
		}
	} else {
		ResetHasCast(use_left_hand);
	}
}

void AnimationEventHandler::ScheduleRightSwitch() {
	auto async_manager = AsyncManager();
	async_manager.AsyncRun(false, is_concentrating_right, has_fired_right);
}

void AnimationEventHandler::ScheduleLeftSwitch() {
	auto async_manager = AsyncManager();
	async_manager.AsyncRun(true, is_concentrating_left, has_fired_left);
}

void AnimationEventHandler::ScheduleBothSwitches() {
	SwitchSpell(false, num_right_casts);
	SwitchSpell(true, num_left_casts);
}

void AnimationEventHandler::CheckFireForgetRightSwitch() {
	has_fired_right = true;
	bool is_left_casting;
	RE::PlayerCharacter::GetSingleton()->GetGraphVariableBool("bWantCastLeft", is_left_casting);
	if (is_left_casting) {
		// Do Nothing
	} else {
		if (has_casted_left) {
			ScheduleLeftSwitch();
		}
		ScheduleRightSwitch();
	}
}

void AnimationEventHandler::CheckFireForgetLeftSwitch() {
	has_fired_left = true;
	bool is_right_casting;
	RE::PlayerCharacter::GetSingleton()->GetGraphVariableBool("bWantCastRight", is_right_casting);
	if (is_right_casting) {
		// Do Nothing
	} else {
		if (has_casted_right) {
			ScheduleRightSwitch();
		}
		ScheduleLeftSwitch();
	}
}

void AnimationEventHandler::CheckFireForgetSwitch(bool use_left_hand)
{
	if (!has_dualcast && (has_casted_left || has_casted_right)) {
		if (use_left_hand) {
			if (is_concentrating_left) {
				has_dualcast = true;
				current_right_spell = current_left_spell;
			} else {
				CheckFireForgetLeftSwitch();
			}
		} else {
			if (is_concentrating_right) {
				has_dualcast = true;
				current_left_spell = current_right_spell;
			} else {
				CheckFireForgetRightSwitch();
			}
		}
	}
}

void AnimationEventHandler::CheckConcentrationSwitch()
{
	if (!has_dualcast) {
		if (has_casted_left && has_casted_right) {
			auto player = RE::PlayerCharacter::GetSingleton();
			bool is_left_casting;
			bool is_right_casting;
			player->GetGraphVariableBool("bWantCastLeft", is_left_casting);
			player->GetGraphVariableBool("bWantCastRight", is_right_casting);
			if (is_left_casting || is_right_casting) {
				// Do nothing
			} else {
				ScheduleBothSwitches();
			}
		} else if (has_casted_left) {
			ScheduleLeftSwitch();
		} else if (has_casted_right) {
			ScheduleRightSwitch();
		}
	} else {
		has_dualcast = false;
		num_right_casts = (num_left_casts > num_right_casts) ? num_left_casts : num_left_casts;
		num_left_casts = (num_right_casts > num_left_casts) ? num_right_casts : num_left_casts;
		ScheduleBothSwitches();
	}
}

void AnimationEventHandler::CheckAnimation(std::string& animation_event) {
	if (animation_event.size() > 0 && valid_events.contains(animation_event)) {
		if (right_cast_begin == animation_event) {
			CheckInitialCast(false);
		} else if (left_cast_begin == animation_event) {
			CheckInitialCast(true);
		} else if (right_concentration_casts.contains(animation_event)) { // For confirming dualcast
			is_concentrating_right = true;
		} else if (left_concentration_casts.contains(animation_event)) { // For confirming dualcast
			is_concentrating_left = true;
		} else if (right_cast_fire_forget == animation_event) {
			CheckFireForgetSwitch(false);
		} else if (left_cast_fire_forget == animation_event) {
			CheckFireForgetSwitch(true);
		} else if (stop_cast == animation_event) {
			CheckConcentrationSwitch();
		}
	}
}