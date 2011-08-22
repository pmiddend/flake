#include "apply_sources.hpp"
#include <mizuiro/color/channel/luminance.hpp>

void
flake::apply_sources(
	flake::scalar_view const &g,
	flake::source_sequence const &s,
	flake::scalar_duration const &dt)
{
	for(
		flake::source_sequence::const_iterator it =
			s.begin();
		it != s.end();
		++it)
	{
		g[it->position()].set(
			mizuiro::color::channel::luminance(),
			g[it->position()].get(mizuiro::color::channel::luminance()) + dt.count() * it->intensity());
	}
}
