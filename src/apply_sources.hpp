#ifndef FLAKE_APPLY_SOURCES_HPP_INCLUDED
#define FLAKE_APPLY_SOURCES_HPP_INCLUDED

#include "scalar_view.hpp"
#include "source_sequence.hpp"
#include "scalar_duration.hpp"

namespace flake
{
void
apply_sources(
	flake::scalar_view const &,
	flake::source_sequence const &,
	flake::scalar_duration const &);
}

#endif
