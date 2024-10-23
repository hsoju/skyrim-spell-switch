#pragma once

struct AsyncManager
{
	void AsyncRun(bool is_left_hand, bool is_concentrating, bool has_fired);
	void AsyncSwitch(bool is_left_hand, bool is_concentrating, bool has_fired);
};