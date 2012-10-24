#ifndef FLAKE_TIME_MODIFIER_KERNEL_HPP_INCLUDED
#define FLAKE_TIME_MODIFIER_KERNEL_HPP_INCLUDED

#include <flake/time_modifier/multiplier.hpp>

namespace flake
{
namespace time_modifier
{
class kernel
{
public:
	explicit
	kernel(
		bool is_paused);

	void
	faster();

	void
	slower();

	void
	toggle_pause();

	time_modifier::multiplier const
	current_multiplier() const;
private:
	time_modifier::multiplier current_multiplier_;
	bool is_paused_;
};
}
}

#endif

