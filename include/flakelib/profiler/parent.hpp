#ifndef FLAKELIB_PROFILER_PARENT_HPP_INCLUDED
#define FLAKELIB_PROFILER_PARENT_HPP_INCLUDED

#include <flakelib/profiler/object_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace profiler
{
FCPPT_MAKE_STRONG_TYPEDEF(
	profiler::object &,
	parent);
}
}

#endif
