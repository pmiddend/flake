#ifndef FLAKELIB_PROFILER_OPTIONAL_PARENT_HPP_INCLUDED
#define FLAKELIB_PROFILER_OPTIONAL_PARENT_HPP_INCLUDED

#include <flakelib/profiler/object_fwd.hpp>
#include <fcppt/optional.hpp>

namespace flakelib
{
namespace profiler
{
typedef
fcppt::optional
<
	profiler::object &
>
optional_parent;
}
}

#endif
