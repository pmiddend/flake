#ifndef FLAKE_APPLY_SOURCES_HPP_INCLUDED
#define FLAKE_APPLY_SOURCES_HPP_INCLUDED

#include "scalar_duration.hpp"
#include "scalar.hpp"
#include "ndim/clamp.hpp"
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/chrono/duration_impl.hpp>

namespace flake
{
template<typename Grid,typename SourceSequence>
void
apply_sources(
	Grid &input,
	SourceSequence const &sources,
	flake::scalar_duration const &diff)
{
	typedef typename
	SourceSequence::const_iterator
	source_iterator;

	for(
		source_iterator it =
			sources.begin();
		it != sources.end();
		++it)
	{
		input[it->position()] =
			ndim::clamp(
				input[it->position()] + diff.count() * it->value(),
				flake::scalar(
					0),
				flake::scalar(
					1));
	}
}
}

#endif
