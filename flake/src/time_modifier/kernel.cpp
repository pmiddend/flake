#include <flake/time_modifier/kernel.hpp>

flake::time_modifier::kernel::kernel(
	bool const _is_paused)
:
	current_multiplier_(
		1),
	is_paused_(
		_is_paused)
{
}

void
flake::time_modifier::kernel::faster()
{
	if(current_multiplier_.get() > 1)
		++current_multiplier_;
	else
		current_multiplier_ *=
			time_modifier::multiplier(2);
}

void
flake::time_modifier::kernel::slower()
{
	if(current_multiplier_.get() > 1)
		--current_multiplier_;
	else
		current_multiplier_ /=
			time_modifier::multiplier(2);
}

void
flake::time_modifier::kernel::toggle_pause()
{
	is_paused_ =
		!is_paused_;
}

flake::time_modifier::multiplier const
flake::time_modifier::kernel::current_multiplier() const
{
	return
		is_paused_
		?
			time_modifier::multiplier(0)
		:
			current_multiplier_;
}
