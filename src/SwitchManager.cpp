#include "SwitchManager.h"

#include "Serialization.h"

void SwitchManager::LogFavoriteSpells()
{
	for (auto spell : switch_spells) {
		logger::info("Spell = {}", spell->fullName.data());
	}
}

void SwitchManager::AddFavoriteSpell(RE::SpellItem* spell, std::unordered_set<RE::SpellItem*>& current_favorites)
{
	if (!switch_set.contains(spell)) {
		switch_set.insert(spell);
		switch_spells.push_back(spell);
	}
	current_favorites.insert(spell);
}

std::unordered_set<RE::SpellItem*> SwitchManager::GetCurrentFavoritedSpells(RE::BSTArray<RE::TESForm*>& favorite_spells)
{
	std::unordered_set<RE::SpellItem*> current_favorites;
	for (auto spell_favorite : favorite_spells) {
		auto form_type = spell_favorite->GetFormType();
		if (form_type == RE::FormType::Spell) {
			//logger::info("{} = {}", spell_favorite->GetName(), spell_favorite->GetFormType());
			RE::SpellItem* spell = spell_favorite->As<RE::SpellItem>();
			auto spell_type = spell->GetSpellType();
			if (spell_type == RE::MagicSystem::SpellType::kSpell) {
				AddFavoriteSpell(spell, current_favorites);
			}
		}
	}
	return current_favorites;
}

std::vector<RE::SpellItem*> SwitchManager::GetUnfavoritedSpells(std::unordered_set<RE::SpellItem*>& current_favorites)
{
	std::vector<RE::SpellItem*> spells_to_remove;
	for (auto old_spell : switch_spells) {
		if (!current_favorites.contains(old_spell)) {
			spells_to_remove.push_back(old_spell);
		}
	}
	return spells_to_remove;
}

void SwitchManager::RemoveUnfavoritedSpells(std::vector<RE::SpellItem*>& spells_to_remove)
{
	for (auto spell : spells_to_remove) {
		switch_spells.erase(std::find(switch_spells.begin(), switch_spells.end(), spell));
		switch_set.erase(spell);
	}
}

void SwitchManager::ProcessFavoriteSpells()
{
	RE::BSTArray<RE::TESForm*>& favorite_spells = RE::MagicFavorites::GetSingleton()->spells;
	std::unordered_set<RE::SpellItem*> current_favorites = GetCurrentFavoritedSpells(favorite_spells);
	std::vector<RE::SpellItem*> spells_to_remove = GetUnfavoritedSpells(current_favorites);
	RemoveUnfavoritedSpells(spells_to_remove);
}

bool SwitchManager::HasSwitchSpell(RE::SpellItem* spell) {
	return switch_set.contains(spell);
}

void SwitchManager::ResetAlternateSpells(bool& chosen_alternate, std::unordered_set<RE::SpellItem*>& chosen_alternate_spells,
	bool activate_alternate) {
	chosen_alternate = activate_alternate;
	chosen_alternate_spells.clear();
}

std::vector<RE::BGSKeyword*> SwitchManager::GetPrimaryKeywords(RE::SpellItem* spell) {
	auto spell_keywords = spell->GetCostliestEffectItem()->baseEffect->GetKeywords();
	std::vector<RE::BGSKeyword*> keywords_vector;

	if (spell_keywords.size() > 0) {
		for (RE::BGSKeyword* spell_keyword : spell_keywords) {
			if (filtered_keywords.contains(std::string(spell_keyword->GetFormEditorID()))) {
				continue;
			}
			keywords_vector.push_back(spell_keyword);
			break;
		}
	}
	return keywords_vector;
}

RE::SpellItem* SwitchManager::FindValidSpellUsingKeywords(RE::SpellItem* spell, int start_idx) {
	std::vector<RE::BGSKeyword*> primary_keywords = GetPrimaryKeywords(spell);
	if (primary_keywords.size() > 0) {
		int current_size = switch_spells.size();
		for (int i = 1; i < current_size; i++) {
			int current_idx = (start_idx + i) % current_size;
			RE::SpellItem* current_spell = switch_spells[current_idx];
			if (current_spell->HasKeywordInArray(primary_keywords, false)) {
				return current_spell;
			}
		}
	}
	return spell;
}


RE::SpellItem* SwitchManager::FindValidSpellUsingMods(RE::SpellItem* spell, int start_idx) {
	RE::TESFile* mod_file = spell->GetFile();
	if (!filtered_mods.contains(mod_file->fileName)) {
		int current_size = switch_spells.size();
		for (int i = 1; i < current_size; i++) {
			int current_idx = (start_idx + i) % current_size;
			RE::SpellItem* current_spell = switch_spells[current_idx];
			if (mod_file == current_spell->GetFile()) {
				return current_spell;
			}
		}
	}
	return spell;
}

RE::SpellItem* SwitchManager::FindValidSpellUsingModsWithSchools(RE::SpellItem* spell, int start_idx) {
	RE::TESFile* mod_file = spell->GetFile();
	if (!filtered_mods.contains(mod_file->fileName)) {
		auto spell_school = spell->GetCostliestEffectItem()->baseEffect->GetMagickSkill();
		std::unordered_set<RE::ActorValue> valid_schools = active_schools.contains(spell_school) ? active_schools : passive_schools;
		int current_size = switch_spells.size();
		for (int i = 1; i < current_size; i++) {
			int current_idx = (start_idx + i) % current_size;
			RE::SpellItem* current_spell = switch_spells[current_idx];
			if (mod_file == current_spell->GetFile() && 
				valid_schools.contains(current_spell->GetCostliestEffectItem()->baseEffect->GetMagickSkill())) {
				return current_spell;
			}
		}
	}
	return spell;
}

RE::SpellItem* SwitchManager::FindValidSpellUsingSchools(RE::SpellItem* spell, int start_idx) {
	auto spell_school = spell->GetCostliestEffectItem()->baseEffect->GetMagickSkill();
	std::unordered_set<RE::ActorValue> valid_schools = active_schools.contains(spell_school) ? active_schools : passive_schools;
	int current_size = switch_spells.size();
	for (int i = 1; i < current_size; i++) {
		int current_idx = (start_idx + i) % current_size;
		RE::SpellItem* current_spell = switch_spells[current_idx];
		if (valid_schools.contains(current_spell->GetCostliestEffectItem()->baseEffect->GetMagickSkill())) {
			return current_spell;
		}
	}
	return spell;
}


RE::SpellItem* SwitchManager::FindValidSpellUsingSchool(RE::SpellItem* spell, int start_idx) {
	auto spell_school = spell->GetCostliestEffectItem()->baseEffect->GetMagickSkill();
	int current_size = switch_spells.size();
	for (int i = 1; i < current_size; i++) {
		int current_idx = (start_idx + i) % current_size;
		RE::SpellItem* current_spell = switch_spells[current_idx];
		if (spell_school == current_spell->GetCostliestEffectItem()->baseEffect->GetMagickSkill()) {
			return current_spell;
		}
	}
	return spell;
}

RE::SpellItem* SwitchManager::FindValidSpellUsingPosition(RE::SpellItem* spell, int start_idx) {
	int current_size = switch_spells.size();
	int current_idx = (start_idx + 1) % current_size;
	RE::SpellItem* current_spell = switch_spells[current_idx];
	return current_spell;
}


RE::SpellItem* SwitchManager::FindValidSpell(RE::SpellItem* spell, int start_idx, bool use_left_hand, int num_casts) {
	bool& current_alternate = use_left_hand ? is_switching_left_alternate : is_switching_right_alternate;
	std::unordered_set<RE::SpellItem*>& previous_alternate_spells = use_left_hand ? 
		previous_left_alternate_spells : previous_right_alternate_spells;
	
	if (current_alternate && previous_alternate_spells.contains(spell)) {
		ResetAlternateSpells(current_alternate, previous_alternate_spells, false);
	}

	if (num_casts > 2) {
		ResetAlternateSpells(current_alternate, previous_alternate_spells, !current_alternate);
	}

	if (current_alternate) {
		previous_alternate_spells.insert(spell);
		RE::SpellItem* chosen_spell = FindValidSpellUsingKeywords(spell, start_idx);
		if (chosen_spell != spell) {
			return chosen_spell;
		}
		chosen_spell = FindValidSpellUsingModsWithSchools(spell, start_idx);
		if (chosen_spell != spell) {
			return chosen_spell;
		}
		ResetAlternateSpells(current_alternate, previous_alternate_spells, false);
		return FindValidSpellUsingSchools(spell, start_idx);
	} else {
		return FindValidSpellUsingSchools(spell, start_idx);
	}
}


RE::SpellItem* SwitchManager::GetNextSpell(RE::SpellItem* spell, bool use_left_hand, int num_casts) {
	if (!HasSwitchSpell(spell)) {
		return spell;
	}
	auto iter = std::find(switch_spells.begin(), switch_spells.end(), spell);
	if (iter != switch_spells.end()) {
		int idx = iter - switch_spells.begin();
		return FindValidSpell(spell, idx, use_left_hand, num_casts);
	}
	return spell;
}


bool SwitchManager::SerializeSave(SKSE::SerializationInterface* a_intfc)
{
	spell_storage.clear();
	for (int idx = 0; idx < switch_spells.size(); idx++) {
		auto spellID = switch_spells[idx]->formID;
		spell_storage.push_back(std::to_string(spellID));
	}

	if (!Serialization::Save(a_intfc, this->spell_storage)) {
		logger::error("Failed to write spell switches");
		return false;
	}
	return true;
}

bool SwitchManager::SerializeSave(SKSE::SerializationInterface* a_intfc, uint32_t a_type, uint32_t a_version)
{
	if (!a_intfc->OpenRecord(a_type, a_version)) {
		logger::error("Failed to open spell switch records!");
		return false;
	}
	return SerializeSave(a_intfc);
}

bool SwitchManager::DeserializeLoad(SKSE::SerializationInterface* a_intfc)
{
	spell_storage.clear();
	if (!Serialization::Load(a_intfc, this->spell_storage)) {
		logger::info("Failed to load spell switches!");
		return false;
	}

	switch_spells.clear();
	switch_set.clear();
	for (auto& item : spell_storage.items()) {
		std::string spell_sID = item.value().get<std::string>();
		auto spell = RE::TESForm::LookupByID(static_cast<RE::FormID>(std::stoul(spell_sID)))->As<RE::SpellItem>();
		switch_spells.push_back(spell);
		switch_set.insert(spell);
	}

	return true;
}

void SwitchManager::Revert()
{
	switch_spells.clear();
	switch_set.clear();
}