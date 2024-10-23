#include "AsyncManager.h"
#include "AnimationEventHandler.h"

void AsyncManager::AsyncRun(bool is_left_hand, bool is_concentrating, bool has_fired)
{
	auto ThreadFunc = [this](bool is_left_hand, bool is_concentrating, bool has_fired) -> void {
		AsyncSwitch(is_left_hand, is_concentrating, has_fired);
	};
	std::jthread thread(ThreadFunc, is_left_hand, is_concentrating, has_fired);
	thread.detach();
}

void AsyncManager::AsyncSwitch(bool is_left_hand, bool is_concentrating, bool has_fired)
{
	float delay = (is_concentrating || !has_fired) ? 50.0f : 500.0f;
	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay)));
	const auto task_interface = SKSE::GetTaskInterface();

	if (task_interface != nullptr) {
		auto player = RE::PlayerCharacter::GetSingleton();
		if (player && player->GetActorRuntimeData().currentProcess &&
			player->GetActorRuntimeData().currentProcess->InHighProcess()) {
			task_interface->AddTask([this, is_left_hand]() -> void {
				auto animation_handler = AnimationEventHandler::GetSingleton();
				int num_casts = is_left_hand ? animation_handler->num_left_casts : animation_handler->num_right_casts;
				animation_handler->SwitchSpell(is_left_hand, num_casts);
			});
		}
	}
}