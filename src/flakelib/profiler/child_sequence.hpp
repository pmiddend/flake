#ifndef FLAKELIB_PROFILER_CHILD_SEQUENCE_HPP_INCLUDED
#define FLAKELIB_PROFILER_CHILD_SEQUENCE_HPP_INCLUDED

#include <flakelib/profiler/object_fwd.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/clone_allocator.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace profiler
{
typedef
boost::ptr_vector
<
	profiler::object,
	boost::view_clone_allocator
>
child_sequence;
}
}

#endif
