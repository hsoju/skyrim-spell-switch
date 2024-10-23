#pragma once

#include <shared_mutex>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class SwitchManager
{
public:
	std::vector<RE::SpellItem*> switch_spells;
	std::unordered_set<RE::SpellItem*> switch_set;

	bool is_switching_right_alternate = false;
	bool is_switching_left_alternate = false;

	std::unordered_set<RE::SpellItem*> previous_right_alternate_spells;
	std::unordered_set<RE::SpellItem*> previous_left_alternate_spells;

	std::unordered_set<RE::ActorValue> active_schools = { RE::ActorValue::kDestruction, RE::ActorValue::kIllusion };
	std::unordered_set<RE::ActorValue> passive_schools = { RE::ActorValue::kRestoration, RE::ActorValue::kConjuration, 
		RE::ActorValue::kAlteration };
	
	std::unordered_set<std::string> filtered_keywords = { "WISpellColorful", "WISpellDangerous", "WB_Destruction" };
	std::unordered_set<std::string> filtered_mods = { "Skyrim.esm", "Dawnguard.esm", "Dragonborn.esm" };

	json spell_storage;
	std::shared_mutex mtx;

	static SwitchManager* GetSingleton()
	{
		static SwitchManager singleton;
		return &singleton;
	}

	void LogFavoriteSpells();
	void AddFavoriteSpell(RE::SpellItem* spell, std::unordered_set<RE::SpellItem*>& current_favorites);
	std::unordered_set<RE::SpellItem*> GetCurrentFavoritedSpells(RE::BSTArray<RE::TESForm*>& favorite_spells);
	std::vector<RE::SpellItem*> GetUnfavoritedSpells(std::unordered_set<RE::SpellItem*>& current_favorites);
	void RemoveUnfavoritedSpells(std::vector<RE::SpellItem*>& spells_to_remove);
	void ProcessFavoriteSpells();

	bool HasSwitchSpell(RE::SpellItem* spell);
	void ResetAlternateSpells(bool& chosen_alternate, std::unordered_set<RE::SpellItem*>& chosen_alternate_spells, 
		bool activate_alternate);

	std::vector<RE::BGSKeyword*> GetPrimaryKeywords(RE::SpellItem* spell);

	RE::SpellItem* FindValidSpellUsingKeywords(RE::SpellItem* spell, int start_idx);
	RE::SpellItem* FindValidSpellUsingMods(RE::SpellItem* spell, int start_idx);
	RE::SpellItem* FindValidSpellUsingModsWithSchools(RE::SpellItem* spell, int start_idx);
	RE::SpellItem* FindValidSpellUsingSchools(RE::SpellItem* spell, int start_idx);
	RE::SpellItem* FindValidSpellUsingSchool(RE::SpellItem* spell, int start_idx);
	RE::SpellItem* FindValidSpellUsingPosition(RE::SpellItem* spell, int start_idx);
	RE::SpellItem* FindValidSpell(RE::SpellItem* spell, int start_idx, bool use_left_hand, int num_casts);
	
	RE::SpellItem* GetNextSpell(RE::SpellItem* spell, bool use_left_hand, int num_casts);

	bool SerializeSave(SKSE::SerializationInterface* a_intfc);
	bool SerializeSave(SKSE::SerializationInterface* a_intfc, uint32_t a_type, uint32_t a_version);
	bool DeserializeLoad(SKSE::SerializationInterface* a_intfc);
	void Revert();

protected:
	SwitchManager() = default;
	SwitchManager(const SwitchManager&) = delete;
	SwitchManager(SwitchManager&&) = delete;
	virtual ~SwitchManager() = default;

	auto operator=(const SwitchManager&) -> SwitchManager& = delete;
	auto operator=(SwitchManager&&) -> SwitchManager& = delete;
};
