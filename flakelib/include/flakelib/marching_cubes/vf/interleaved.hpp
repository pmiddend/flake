#ifndef FLAKELIB_MARCHING_CUBES_VF_INTERLEAVED_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_VF_INTERLEAVED_HPP_INCLUDED

#include <flakelib/marching_cubes/vf/interleaved_part.hpp>
#include <sge/renderer/vf/format.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace marching_cubes
{
namespace vf
{
typedef
sge::renderer::vf::format
<
	boost::mpl::vector1
	<
		vf::interleaved_part
	>
>
interleaved;
}
}
}

#endif
