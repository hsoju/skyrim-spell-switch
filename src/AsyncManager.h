#pragma once

struct AsyncManager
{
	float GetDelay(bool is_concentrating, bool has_fired);
	void AsyncRun(bool is_left_hand, bool is_concentrating, bool has_fired);
	void AsyncSwitch(bool is_left_hand, bool is_concentrating, bool has_fired);
};