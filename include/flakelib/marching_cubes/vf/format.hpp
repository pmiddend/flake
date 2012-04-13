#ifndef FLAKELIB_MARCHING_CUBES_VF_FORMAT_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_VF_FORMAT_HPP_INCLUDED

#include <flakelib/marching_cubes/vf/normal_part.hpp>
#include <flakelib/marching_cubes/vf/position_part.hpp>
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
	boost::mpl::vector2
	<
		vf::position_part,
		vf::normal_part
	>
>
format;
}
}
}

#endif

